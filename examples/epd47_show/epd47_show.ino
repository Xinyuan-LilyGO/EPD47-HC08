#include "driver/i2s.h"
#include <HardwareSerial.h>
#include "memo_historyManager.h"
#include "hz3500_36.h"
#include "time.h"


//-----------------------PDM麦克风参数-------------------------//
#define READ_DELAY 2000 // millisec
const i2s_port_t I2S_PORT = I2S_NUM_0;
const int BLOCK_SIZE = 512;
int samples[BLOCK_SIZE];
#define SAMPLE_RATE 24000
long total_read = 0;
long noiseVal = 0;
#define MIC_PDM_DATA 12 //PDM microphone DATA pin 12
#define MIC_PDM_CLOCK 13//PDM microphone CLOCK pin 13

//-----------------------HC-08参数------------------------------//
#define HC08_TX 15
#define HC08_RX 14
HardwareSerial MySerial(1);

//-----------------------ESP32墨水瓶参数-------------------------//

#define BUTTON_BOOT_PRESS  39
uint8_t *framebuffer;
uint32_t blue_receive_txt_time = 0;
uint32_t wakeup_time = 0;
String blue_receive_txt = "";
String buff_command[5];
bool pin_link = false;
bool init_hard_ok = false;
memo_historyManager* objmemo_historyManager;
esp_sleep_wakeup_cause_t wakeup_reason;
hw_timer_t *timer = NULL;


bool check_pin()
{
  pinMode(BUTTON_BOOT_PRESS, INPUT);
  delay(500);
  int pin_link = digitalRead(BUTTON_BOOT_PRESS) ;
  if (pin_link == 0)
    return true;
  else
    return false;
}

//------------------初始化硬件及变量------------------//
void init_hard(bool load_history)
{


  //初始化SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS init failed");
    Serial.println("SPIFFS format ...");
    if (SPIFFS.format())
    {
      Serial.println("SPIFFS format ok");
      Serial.println("SPIFFS re_init");
      if (SPIFFS.begin(true))
      {
      }
      else
      {
        Serial.println("SPIFFS re_init error");
        ets_printf("reboot\n");
        esp_restart();
        return;
      }
    }
    else
    {
      Serial.println("SPIFFS format failed");
      ets_printf("reboot\n");
      esp_restart();
      return;
    }
  }
  Serial.println("SPIFFS init ok");

  if (pin_link)
  {
    if (SPIFFS.exists(TXTDATA_FILENAME)) {
      SPIFFS.remove(TXTDATA_FILENAME);
      Serial.println(String(TXTDATA_FILENAME) + " remove");
    }
  }
  else
  {
    if (load_history)
      objmemo_historyManager->load_list();
  }
  Serial.println("init_hard ok");
}

//------------------字符显示------------------//
int GetCharwidth(String ch)
{
  //修正，空格计算的的宽度为0, 强制36 字体不一样可能需要修改！
  if (ch == " ") return 28;

  char buf[50];
  int x1 = 0, y1 = 0, w = 0, h = 0;
  int tmp_cur_x = 0;
  int tmp_cur_y = 0;
  FontProperties properties;
  get_text_bounds((GFXfont *)&msyh36, (char *) ch.c_str(), &tmp_cur_x, &tmp_cur_y, &x1, &y1, &w, &h, &properties);
  //sprintf(buf, "x1=%d,y1=%d,w=%d,h=%d", x1, y1, w, h);
  //Serial.println("ch="+ ch + ","+ buf);

  //负数说明没找到这个字,会不显示出来
  if (w <= 0)
    w = 0;
  return (w);
}

//------------------文字显示------------------//
void Show_hz(String rec_text, bool loadbutton)
{
  //最长限制160字节，40汉字
  //6个字串，最长约在 960字节，小于1024, json字串最大不超过1024
  rec_text = rec_text.substring(0, 160);
  Serial.println("begin Showhz:" + rec_text);

  //更正汉字符号显示的bug
  rec_text.replace("，", ",");
  rec_text.replace("。", ".");
  rec_text.replace("？", "?");

  epd_poweron();
  //uint32_t t1 = millis();
  //全局刷
  epd_clear();
  //局刷,一样闪屏
  //epd_clear_area(screen_area);
  //epd_full_screen()

  //此句不要缺少，否则显示会乱码
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

  //uint32_t t2 = millis();
  //printf("EPD clear took %dms.\n", t2 - t1);
  int cursor_x = 10;
  int cursor_y = 80;

  //多行文本换行显示算法。
  if (!loadbutton)
    objmemo_historyManager->multi_append_txt_list(rec_text);

  String now_string = "";
  int i;
  //write_string 能根据手工加的 "\n"换行，但不能自由控制行距，此处我自行控制了.
  for ( i = 0; i < objmemo_historyManager->memolist.size() ; i++)
  {
    now_string = objmemo_historyManager->memolist.get(i);
    //Serial.println("Show_hz line:" + String((now_index + i) % TXT_LIST_NUM) + " " + now_string);

    if (now_string.length() > 0)
    {
      //加">"字符，规避epd47的bug,当所有字库不在字库时，esp32会异常重启
      // “Guru Meditation Error: Core 1 panic'ed (LoadProhibited). Exception was unhandled."
      now_string = ">" + now_string;
      //墨水屏writeln不支持自动换行
      //delay(200);
      //一定要用framebuffer参数，否则当最后一行数据过长时，会导致代码在此句阻塞，无法休眠，原因不明！

      writeln((GFXfont *)&msyh36, (char *)now_string.c_str(), &cursor_x, &cursor_y, framebuffer);
      //writeln调用后，cursor_x会改变，需要重新赋值
      cursor_x = 10;
      cursor_y = cursor_y + 85;
    }
  }

  //前面不要用writeln，有一定机率阻塞，无法休眠
  epd_draw_grayscale_image(epd_full_screen(), framebuffer);

  //delay(500);
  epd_poweroff();

  Serial.println("end Showhz:" + rec_text );
}

//------------------串口清除缓存------------------//
void clear_uart(int ms_time)
{
  //唤醒完成后就可以正常接收串口数据了
  uint32_t starttime = 0;
  char ch;
  //5秒内有输入则输出
  starttime = millis();
  //临时接收缓存，防止无限等待
  while (true)
  {
    if  (millis()  - starttime > ms_time)
      break;
    while (MySerial.available())
    {
      ch = (char) MySerial.read();
      Serial.print(ch);
    }
    yield();
    delay(5);
  }
}

//------------------休眠------------------//
void goto_sleep()
{
  Serial.println("sleep...");
  delay(100);

  //不调用此句无法节能, 此句必须配合esp32深度休眠,轻度休眠无效！
  epd_poweroff_all();
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 0);
  esp_deep_sleep_start();
}

// http://www.schwietering.com/jayduino/filtuino/index.php?characteristic=be&passmode=hp&order=2&usesr=usesr&sr=24000&frequencyLow=100&noteLow=&noteHigh=&pw=pw&calctype=float&run=Send
//High pass bessel filter order=2 alpha1=0.0041666666666667
class  FilterBeHp2
{
  public:
    FilterBeHp2()
    {
      v[0] = 0.0;
      v[1] = 0.0;
    }
  private:
    float v[3];
  public:
    float step(float x) //class II
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (9.823849154958753660e-1 * x)
             + (-0.96497792085018785357 * v[0])
             + (1.96456174113331383246 * v[1]);
      return
        (v[0] + v[2])
        - 2 * v[1];
    }
};
FilterBeHp2 filter;

//----------------PDM麦克风初始化----------------//
void init_pdm() {

  esp_err_t err;

  i2s_config_t audio_in_i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // although the SEL config should be left, it seems to transmit on right
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
    .dma_buf_count = 4,
    .dma_buf_len = BLOCK_SIZE
  };

  // This function must be called before any I2S driver read/write operations.
  err = i2s_driver_install(I2S_PORT, &audio_in_i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }

  // configure which pin on the ESP32 is connected to which pin on the mic:
  i2s_pin_config_t audio_in_pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE, // not used
    .ws_io_num = MIC_PDM_CLOCK,  // clock pin
    .data_out_num = I2S_PIN_NO_CHANGE, // Not used
    .data_in_num = MIC_PDM_DATA   // data pin
  };

  err = i2s_set_pin(I2S_PORT, &audio_in_pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
}

//-----------------麦克风检测--------------------//
long process_samples() {

  long val = 0;
  while (val < 3000||val == 30934) {
    int num_bytes_read = i2s_read_bytes(I2S_PORT,
                                        (char *)samples,
                                        BLOCK_SIZE,     // the doc says bytes, but its elements.
                                        portMAX_DELAY); // no timeout
    if (num_bytes_read > 0) {

      int samples_read = num_bytes_read / 4;
      total_read += samples_read;
      float sample;
      for (int i = 0; i < samples_read; i++) {
        sample = filter.step((float)samples[i] / INT_MAX);
        sample = (float)samples[i] / INT_MAX;
        Serial.println((short)abs((sample * SHRT_MAX)));
        val = abs((sample * SHRT_MAX));

      }
    }
  }
  return val;
}


void setup() {
  Serial.begin(115200);
  //声音初始化
  init_pdm();

  pin_link = check_pin();
  wakeup_time = millis() / 1000;
  blue_receive_txt_time = 0;
  blue_receive_txt = "";
  wakeup_reason = esp_sleep_get_wakeup_cause();
  init_hard_ok = false;
  //水墨屏硬件初始化
  epd_init();

  framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!framebuffer)
  {
    Serial.println("alloc memory failed !!!");
    delay(1000);
    while (1);
  }
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);


  objmemo_historyManager = new memo_historyManager();
  objmemo_historyManager->GetCharwidth = GetCharwidth;
  if (pin_link)
  {
    Serial.println("init_hard");
    init_hard(false);
    init_hard_ok = true;
    //Show_hz("", true);
  }

  MySerial.begin(9600, SERIAL_8N1, HC08_RX, HC08_TX);
  //蓝牙串口唤醒的字串不能直接用，前面几个字节的数据会乱码，需要丢掉
  //需要约定，先随意发一串数据，过>300ms后再正式发送信息
  clear_uart(100);
  Serial.println("start");
  Serial.println("wakeup_reason=" + String(wakeup_reason));
  if (wakeup_reason != ESP_SLEEP_WAKEUP_EXT0)
  {
    delay(100);
    Show_hz("休眠开始，等待激活", false);
    goto_sleep();
  }
}



void loop() {
  if (millis() / 1000 < wakeup_time)
    wakeup_time = millis() / 1000;

  while (MySerial.available())
  {
    String tmpstr = "";
    char ch;
    //串口数据加延时能减少一个字串会拆成好几次收到的机率
    //现在通过结束符>>> 确保一次收到完整的字串
    while (MySerial.available())
    {
      ch = (char) MySerial.read();
      tmpstr = tmpstr + ch;
      //Serial.print(ch);
      delay(30);
    }

    //记录首次收到部分字符的时间
    if (blue_receive_txt.length() == 0)
      blue_receive_txt_time = millis() / 1000 ;
    Serial.println(">>" + tmpstr);
    blue_receive_txt = blue_receive_txt + tmpstr;
  }

  if (blue_receive_txt.length() > 0 && blue_receive_txt_time != 0 &&
      (  millis() / 1000 - blue_receive_txt_time > 60 ||  blue_receive_txt.endsWith(">>>") )
     )
  {

    blue_receive_txt.replace(">>>", "");
    Serial.println(">>>" + blue_receive_txt);
    //wakeup_time = millis() / 1000; //工作状态，休眠时间重新计时

    //timerWrite(timer, 0); //reset timer (feed watchdog)



    //唤醒专用命令字符，不显示
    // 因蓝牙空中传播，发出的字串可能会分多次收到， 用>>> 表示字符结束，
    Serial.println("blue_receive_txt len=" + String(blue_receive_txt.length()));

    //初始化硬件驱动，收到首条文字后再做，节省资源
    if (init_hard_ok == false)
    {
      Serial.println("init_hard");
      init_hard(true);
      init_hard_ok = true;
    }
    Show_hz(blue_receive_txt, false);
    objmemo_historyManager->save_list();
    MySerial.print(String("ok: len=") + String(blue_receive_txt.length()) + ">>>");

    //一般约10-30汉字4-5秒左右
    Serial.println("处理文字用时:" + String(millis() / 1000 - wakeup_time) + "秒！");
    blue_receive_txt = "";
    Show_hz("请对麦克风吹气", false);
    noiseVal = process_samples();
    Show_hz("麦克风正常收音，休眠开始", false);
    goto_sleep();
  }
  //delay(READ_DELAY);
  delay(10);
}
