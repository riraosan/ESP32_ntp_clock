#include <Arduino.h>
#include <unity.h>
#include <ESP32_SPIFFS_ShinonomeFNT.h>
#include <ESP32_SPIFFS_UTF8toSJIS.h>

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

#define PORT_SE_IN 13
#define PORT_AB_IN 27
#define PORT_A3_IN 23
#define PORT_A2_IN 21
#define PORT_A1_IN 25
#define PORT_A0_IN 26
#define PORT_DG_IN 19
#define PORT_CLK_IN 18
#define PORT_WE_IN 17
#define PORT_DR_IN 16
#define PORT_ALE_IN 22

#define PANEL_NUM 2
#define R 1
#define O 2
#define G 3

const char *UTF8SJIS_file = "/Utf8Sjis.tbl";        //UTF8 Shift_JIS 変換テーブルファイル名を記載しておく
const char *Shino_Zen_Font_file = "/shnmk16.bdf";   //全角フォントファイル名を定義
const char *Shino_Half_Font_file = "/shnm8x16.bdf"; //半角フォントファイル名を定義

ESP32_SPIFFS_ShinonomeFNT SFR;

void setAllPortOutput()
{
  pinMode(PORT_SE_IN, OUTPUT);
  pinMode(PORT_AB_IN, OUTPUT);
  pinMode(PORT_A3_IN, OUTPUT);
  pinMode(PORT_A2_IN, OUTPUT);
  pinMode(PORT_A1_IN, OUTPUT);
  pinMode(PORT_A0_IN, OUTPUT);
  pinMode(PORT_DG_IN, OUTPUT);
  pinMode(PORT_CLK_IN, OUTPUT);
  pinMode(PORT_WE_IN, OUTPUT);
  pinMode(PORT_DR_IN, OUTPUT);
  pinMode(PORT_ALE_IN, OUTPUT);
}

void setAllPortLow()
{
  digitalWrite(PORT_SE_IN, LOW);
  digitalWrite(PORT_AB_IN, LOW);
  digitalWrite(PORT_A3_IN, LOW);
  digitalWrite(PORT_A2_IN, LOW);
  digitalWrite(PORT_A1_IN, LOW);
  digitalWrite(PORT_A0_IN, LOW);
  digitalWrite(PORT_DG_IN, LOW);
  digitalWrite(PORT_CLK_IN, LOW);
  digitalWrite(PORT_WE_IN, LOW);
  digitalWrite(PORT_DR_IN, LOW);
  digitalWrite(PORT_ALE_IN, LOW);
}

//********************************************************************
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
//********************************************************************
void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}

void setRAMAdder(uint8_t lineNumber)
{
  uint8_t A[4] = {0};
  uint8_t adder = 0;

  adder = lineNumber;

  for (int i = 0; i < 4; i++)
  {
    A[i] = adder % 2;
    adder /= 2;
  }

  digitalWrite(PORT_A0_IN, A[0]);
  digitalWrite(PORT_A1_IN, A[1]);
  digitalWrite(PORT_A2_IN, A[2]);
  digitalWrite(PORT_A3_IN, A[3]);
}

////////////////////////////////////////////////////////////////////////////////////
//データを1行だけ書き込む
//
//iram_addr:データを書き込むアドレス（0~15）
//ifont_data:フォント表示データ(32*PANEL_NUM bit)
//color_data:フォント表示色配列:（32*PANEL_NUM bit）Red:1 Orange:2 Green:3
//
////////////////////////////////////////////////////////////////////////////////////
void send_line_data(uint8_t iram_adder, uint8_t ifont_data[], uint8_t color_data[])
{

  uint8_t font[8] = {0};
  uint8_t tmp_data = 0;
  int k = 0;

  for (int j = 0; j < 4 * PANEL_NUM; j++)
  {
    //ビットデータに変換
    tmp_data = ifont_data[j];
    for (int i = 0; i < 8; i++)
    {
      font[i] = tmp_data % 2;
      tmp_data /= 2;
    }

    for (int i = 7; i >= 0; i--)
    {
      digitalWrite(PORT_DG_IN, LOW);
      digitalWrite(PORT_DR_IN, LOW);
      digitalWrite(PORT_CLK_IN, LOW);

      //TEST_ASSERT_EQUAL(G, color_data[k]);

      if (font[i] == 1)
      {
        if (color_data[k] == R)
        {
          digitalWrite(PORT_DR_IN, HIGH);
        }

        if (color_data[k] == G)
        {
          digitalWrite(PORT_DG_IN, HIGH);
        }

        if (color_data[k] == O)
        {
          digitalWrite(PORT_DR_IN, HIGH);
          digitalWrite(PORT_DG_IN, HIGH);
        }
      }
      else
      {
        digitalWrite(PORT_DR_IN, LOW);
        digitalWrite(PORT_DG_IN, LOW);
      }

      delayMicroseconds(1);
      digitalWrite(PORT_CLK_IN, HIGH);
      delayMicroseconds(1);

      k++;
    }
  }
  //アドレスをポートに入力
  setRAMAdder(iram_adder);
  //ALE　Highでアドレスセット
  digitalWrite(PORT_ALE_IN, HIGH);
  //WE Highでデータを書き込み
  digitalWrite(PORT_WE_IN, HIGH);
  //WE Lowをセット
  digitalWrite(PORT_WE_IN, LOW);
  //ALE Lowをセット
  digitalWrite(PORT_ALE_IN, LOW);
}

///////////////////////////////////////////////////////////////
//配列をnビット左へシフトする関数
//dist:格納先の配列
//src:入力元の配列
//len:配列の要素数
//n:一度に左シフトするビット数
///////////////////////////////////////////////////////////////
void shift_bit_left(uint8_t dist[], uint8_t src[], int len, int n)
{
  uint8_t mask = 0xFF << (8 - n);
  for (int i = 0; i < len; i++)
  {
    if (i < len - 1)
    {
      dist[i] = (src[i] << n) | ((src[i + 1] & mask) >> (8 - n));
    }
    else
    {
      dist[i] = src[i] << n;
    }
  }
}

void shift_color_left(uint8_t dist[], uint8_t src[], int len)
{
  for (int i = 0; i < len * 8; i++)
  {
    if (i < len * 8 - 1)
    {
      dist[i] = src[i + 1];
    }
    else
    {
      dist[i] = 0;
    }
  }
}

//半角4バイトのフォントをスクロールしながら表示するメソッド
//
//sj_length:半角文字数
//font_data:フォントデータ（東雲フォント）
//color_data:フォントカラーデータ（半角毎に設定する）
//intervals:スクロールスピード(ms)
void scrollLEDMatrix(int16_t sj_length, uint8_t font_data[][16], uint8_t color_data[], uint16_t intervals)
{
  uint8_t src_line_data[sj_length] = {0};
  uint8_t dist_line_data[sj_length] = {0};
  uint8_t tmp_color_data[sj_length * 8] = {0};
  uint8_t tmp_font_data[sj_length][16] = {0};
  uint8_t ram = LOW;

  int n = 0;
  for (int i = 0; i < sj_length; i++)
  {

    //8ビット毎の色情報を1ビット毎に変換する
    for (int j = 0; j < 8; j++)
    {
      tmp_color_data[n++] = color_data[i];
    }

    //フォントデータを作業バッファにコピー
    for (int j = 0; j < 16; j++)
    {
      tmp_font_data[i][j] = font_data[i][j];
    }
  }

  for (int k = 0; k < sj_length * 8 + 2; k++)
  {
    ram = ~ram;
    digitalWrite(PORT_AB_IN, ram); //RAM-A/RAM-Bに書き込み
    for (int i = 0; i < 16; i++)
    {
      for (int j = 0; j < sj_length; j++)
      {
        //フォントデータをビットシフト元バッファにコピー
        src_line_data[j] = tmp_font_data[j][i];
      }

      send_line_data(i, src_line_data, tmp_color_data);
      shift_bit_left(dist_line_data, src_line_data, sj_length, 1);

      //font_dataにシフトしたあとのデータを書き込む
      for (int j = 0; j < sj_length; j++)
      {
        tmp_font_data[j][i] = dist_line_data[j];
      }
    }
    shift_color_left(tmp_color_data, tmp_color_data, sj_length);
    delay(intervals);
  }
}

void test_send_line_data(void)
{

  uint16_t sj_length = 0;         //半角文字数
  uint8_t font_buf[26][16] = {0}; //フォントデータバッファ

  //フォント色データ　str1（半角文字毎に設定する）
  uint8_t font_color1[26] = {R, R, G, G, G, G, R, G, G, G, G, R, G, G, G, G, R, G, G, G, G, R, O, O, O, O};
  //uint8_t font_color1[8] = {R,R,R,R,G,G,G,G};
  //フォント色データ　str2（半角文字毎に設定する）
  //uint8_t font_color2[15] = {R,R,O,O,O,O,G,G,O,O,G,G,G,O,R};

  String str1 = "  明治>大正>昭和>平成>令和"; //表示文字列
  //String str2 = "  午後12時 57分";
  //String str1 = "平成令和";
  digitalWrite(PORT_SE_IN, HIGH);

  //String 文字列から一気にフォント変換
  sj_length = SFR.StrDirect_ShinoFNT_readALL(str1, font_buf);
  TEST_ASSERT_EQUAL(26, sj_length);
  //80ms毎に左へ1ビットづつスクロールする
  scrollLEDMatrix(sj_length, font_buf, font_color1, 80);

  //sj_length = SFR.StrDirect_ShinoFNT_readALL(str2, font_buf);
  //TEST_ASSERT_EQUAL(15, sj_length);
  //scrollLEDMatrix(sj_length, font_buf, font_color2, 80);
}

void test_shift_bit(void)
{

  uint8_t src_font[3] = {0x30, 0xf0, 0x80};
  uint8_t dist_font[3] = {0};

  shift_bit_left(dist_font, src_font, 3, 1);

  TEST_ASSERT_EQUAL(0x61, dist_font[0]);
  TEST_ASSERT_EQUAL(0xE1, dist_font[1]);
  TEST_ASSERT_EQUAL(0x00, dist_font[2]);
}

void test_font(void)
{

  uint8_t font_buf[2][16] = {0};
  uint16_t sj_length;
  String str = "あ";

  sj_length = SFR.StrDirect_ShinoFNT_readALL(str, font_buf); //String 文字列から一気にフォント変換

  TEST_ASSERT_EQUAL(2, sj_length);

  TEST_ASSERT_EQUAL(0x0200, font_buf[0][0] << 8 | font_buf[0 + 1][0]);
  TEST_ASSERT_EQUAL(0x0200, font_buf[0][1] << 8 | font_buf[0 + 1][1]);
  TEST_ASSERT_EQUAL(0x0260, font_buf[0][2] << 8 | font_buf[0 + 1][2]);
  TEST_ASSERT_EQUAL(0x1f80, font_buf[0][3] << 8 | font_buf[0 + 1][3]);
  TEST_ASSERT_EQUAL(0x0200, font_buf[0][4] << 8 | font_buf[0 + 1][4]);
  TEST_ASSERT_EQUAL(0x0240, font_buf[0][5] << 8 | font_buf[0 + 1][5]);
  TEST_ASSERT_EQUAL(0x03f0, font_buf[0][6] << 8 | font_buf[0 + 1][6]);
  TEST_ASSERT_EQUAL(0x0448, font_buf[0][7] << 8 | font_buf[0 + 1][7]);
  TEST_ASSERT_EQUAL(0x0c44, font_buf[0][8] << 8 | font_buf[0 + 1][8]);
  TEST_ASSERT_EQUAL(0x1482, font_buf[0][9] << 8 | font_buf[0 + 1][9]);
  TEST_ASSERT_EQUAL(0x2482, font_buf[0][10] << 8 | font_buf[0 + 1][10]);
  TEST_ASSERT_EQUAL(0x2302, font_buf[0][11] << 8 | font_buf[0 + 1][11]);
  TEST_ASSERT_EQUAL(0x2504, font_buf[0][12] << 8 | font_buf[0 + 1][12]);
  TEST_ASSERT_EQUAL(0x1818, font_buf[0][13] << 8 | font_buf[0 + 1][13]);
  TEST_ASSERT_EQUAL(0x0060, font_buf[0][14] << 8 | font_buf[0 + 1][14]);
  TEST_ASSERT_EQUAL(0x0000, font_buf[0][15] << 8 | font_buf[0 + 1][15]);

  for (int i = 0; i < sj_length; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      Serial.printf("0x%02x%02x\n", font_buf[i][j], font_buf[i + 1][j]);
    }
  }
}

void setup()
{

  delay(2000);
  Serial.begin(115200);

  setAllPortOutput();
  setAllPortLow();

  Serial.println("----------------------");

  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  Serial.printf("Flash Chip Size = %d byte\r\n", ESP.getFlashChipSize());

  delay(100);

  //deleteFile(SPIFFS, "/xxx.txt");

  listDir(SPIFFS, "/", 0); //SPIFFSフラッシュ　ルートのファイルリスト表示

  SPIFFS.end();

  SFR.SPIFFS_Shinonome_Init3F(UTF8SJIS_file, Shino_Half_Font_file, Shino_Zen_Font_file);

  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(test_shift_bit);
  RUN_TEST(test_send_line_data);

  SFR.SPIFFS_Shinonome_Close3F();

  UNITY_END(); // stop unit testing
}

void loop()
{
}


#ifdef ESP32_BLE

static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    log_i("Notify callback for characteristic %s of data length %d", pBLERemoteCharacteristic->getUUID().toString().c_str(), length);
}

class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
        log_i("onConnect");
        message = MESSAGE::MSG_COMMAND_BLE_CONNECTED;
    }

    void onDisconnect(BLEClient *pclient)
    {
        log_i("onDisconnect");
        message = MESSAGE::MSG_COMMAND_BLE_DISCONNECTED;
    }
};

bool connectToServer(BLEAddress pAddress)
{
    log_i("Forming a connection to %s", pAddress.toString().c_str());

    BLEClient *pClient = BLEDevice::createClient();
    log_i(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    log_i(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr)
    {
        log_i("Failed to find our service UUID: %s", serviceUUID.toString().c_str());
        return false;
    }
    log_i(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr)
    {
        log_i("Failed to find our characteristic UUID: %s", charUUID.toString().c_str());
        return false;
    }

    log_i(" - Found our characteristic");

    // Read the value of the characteristic.
    std::string value = pRemoteCharacteristic->readValue();
    log_i("The characteristic value was: %s", value.c_str());

    pRemoteCharacteristic->registerForNotify(notifyCallback);

    return true;
}

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{

    // Called for each advertising BLE server.
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        log_i("BLE Advertised Device found: %s", advertisedDevice.toString().c_str());

        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID))
        {
            log_i("Found our device!  address: %s", advertisedDevice.getAddress().toString().c_str());
            advertisedDevice.getScan()->stop();

            pServerAddress = new BLEAddress(advertisedDevice.getAddress());
            message = MESSAGE::MSG_COMMAND_BLE_DO_CONNECT;

        } // Found our server
        else
        {
            message = MESSAGE::MSG_COMMAND_BLE_NOT_FOUND;
        } // Not found our server
    }     // onResult
};        // MyAdvertisedDeviceCallbacks

static MyAdvertisedDeviceCallbacks *pAdvertisedDeviceCallback;

void initBLE()
{
    log_i("Starting Arduino BLE Client application...");

    BLEDevice::deinit();

    BLEDevice::init("");
    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 30 seconds.
    BLEScan *pBLEScan = BLEDevice::getScan();
    pAdvertisedDeviceCallback = new MyAdvertisedDeviceCallbacks();

    pBLEScan->setAdvertisedDeviceCallbacks(pAdvertisedDeviceCallback);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30); //waitting to find BLE server
}

#endif

