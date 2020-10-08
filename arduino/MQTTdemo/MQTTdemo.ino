/*
 * ==================================================================================
 * 
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * File_name : MQTTdemo.ino
 * Describe  : MQTT使用模板
 * Author    : GHJ
 * Date      : 2020年07月05日 13:40:00
 * 
 * PS:
 *    需要MQTT->PubSubClient库:https://github.com/knolleary/pubsubclient
 *    需要SHA256->Crypto库:https://github.com/OperatorFoundation/Crypto.git
 *    
 * 
 * Change Logs:
 * Date              Author        Notes
 * 2020年07月05日     GHJ          the first version
 * 2020年07月14日     GHJ          add mqtt to connect EMQX
 * 2020年10月05日     GHJ          add Aliyun MQTT rules to connect Aliyun IoT
 * 2020年10月06日     GHJ          add AliyunIoT function to connect Aliyun IoT 
 * 
 * 
 * ===================================================================================
 */

/*=========================== 头文件 =============================*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Crypto.h>
#include <SHA256.h>
#include <string.h>
/*=========================== 全局变量及宏定义 =============================*/
/**
 * WiFiClient 创建wifi 客户端对象,用于接入WIFI
 * PubSubClient 创建mqtt 客户端对象,用于接入MQTT
 * ssid : WIFI名称
 * password : WIFI密码
 * port : 端口 通常1883 不需要改
 */

#define port 1883
const char *ssid = "heartbeats";
const char *password = "24682468";
WiFiClient espClient;
PubSubClient client(espClient);
/** if you want to use Aliyun you can use the following instructions.
 *  
 *  STEP 1 定义宏定义 #define Aliyun_IoT
 *            such as  //#define Aliyun_IoT -> #define Aliyun_IoT
 *  STEP 2 设置三元组信息 
 *            such as     String productKey = "a1AiKMtUf57";   //产品Key
 *                        String deviceName = "demo";          //设备名
 *                        String deviceSecret = "77dcf17f5bf94e05f370a55ae2fc085b"; //设备密码  
 *  PS : 默认使用用户私人的云平台，请自行配置
 */ 
#define Aliyun_IoT
#ifdef Aliyun_IoT
/********************************* 阿里云IoT 物联网云平台 ********************************/
#define SHA256HMAC_SIZE 32
/*三元组*/
String productKey = "a1AiKMtUf57";                        //产品Key
String deviceName = "demo";                               //设备名
String deviceSecret = "77dcf17f5bf94e05f370a55ae2fc085b"; //设备密码
/*以下信息由AliyunIoT()接口实现,不必手动修改*/
//${YourProductKey}.iot-as-mqtt.${YourRegionId}.aliyuncs.com
String aliyun_mqtt_server = "YourProductKey.iot-as-mqtt.cn-shanghai.aliyuncs.com";
//${clientId}|securemode=3,signmethod=hmacsha1|
//${clientId}为设备的ID信息。可取任意值，长度在64字符以内。建议使用设备的MAC地址或SN码。
String aliyun_clientID = "clientId|securemode=3,signmethod=hmacsha1|";
//${YourDeviceName}&${YourProductKey}
String aliyun_clientName = "YourDeviceName&YourProductKey";
String aliyun_clientPwd = "3BE07969F6995653F4FB53A86878DF0BB0928576";
/**点击了解更多**/
/**https://help.aliyun.com/document_detail/140507.html?spm=a2c4g.11186623.6.571.1b5a24dfCbOOFK**/
/********************************* 阿里云IoT 物联网云平台 ********************************/
#else
/*
 * mqtt_server : MQTT服务器域名/IP地址
 * clientID : 接入服务器的MQTT客户端ID
 * clientName ：接入服务器的用户
 * clientPwd ：接入服务器的密码
 */
const char *mqtt_server = "47.95.249.141";
const char *clientID = "mqtt-31902342";
const char *clientName = "ESP32";
const char *clientPwd = "public";
#endif
/*=========================== 用户自定义全局变量&宏定义 =============================*/

#define LED_1 16 //GPIO16
#define LED_2 2  //GPIO02

/*=========================== 用户自定义全局变量&宏定义 =============================*/

/*======================================== WIFI =========================================*/
/*
 * @brief:连接WiFi 
 * @param:none
 * @retval:none
*/
void wifi_connect()
{
  //设置wifi station模式
  WiFi.mode(WIFI_STA);
  //连接wifi
  WiFi.begin(ssid, password);
  //判断连接状态
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("WIFI is connecting ...");
  }
  Serial.println("WIFI connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*======================================== MQTT =========================================*/

/*
 * @brief:MQTT回调函数,用于获取订阅的消息
 * @param:topics 订阅的主题
 * @param:payload 订阅的主题的消息 注：byte 只能一个字节一个字节读
 * @param:length 消息大小
 * @retval:none
*/
void callback(char *topics, byte *payload, unsigned int length)
{
  String topic = topics; //Stirng方便处理
  /**打印消息**/
  Serial.print("Message arrived [");
  Serial.print(topics);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  /**对消息进行处理,用户自己完成**/
  /**===================== Example ====================**/
  /*
   * 可以对 topic 筛选  
  */

  if (topic == "/LED_ON")
  {
    digitalWrite(LED_2, LOW); //开灯
  }
  if (topic == "/LED_OFF")
  {
    digitalWrite(LED_2, HIGH); //关灯
  }
  /**===================== Example ====================**/
}
#ifdef Aliyun_IoT
/*====================================== Aliyun_IOT =========================================*/
/*
 * @brief:hmac256 加密 用于计算Aliyun接入密码
 * @param: signcontent 加密对象 拼接规则 ${"clientId" + deviceName + "deviceName" + deviceName + "productKey" + productKey }
 * @param: deviceSecret 设备密码用于加密   
 * @retval: Password 接入Aliyun的密码 
*/
String hmac256(const String signcontent, const String deviceSecret)
{
  byte hashCode[SHA256HMAC_SIZE];
  SHA256 sha256;

  const char *key = deviceSecret.c_str();
  size_t keySize = deviceSecret.length();

  sha256.resetHMAC(key, keySize);
  sha256.update((const byte *)signcontent.c_str(), signcontent.length());
  sha256.finalizeHMAC(key, keySize, hashCode, sizeof(hashCode));

  String Password = "";
  for (byte i = 0; i < SHA256HMAC_SIZE; ++i)
  {
    Password += "0123456789ABCDEF"[hashCode[i] >> 4];
    Password += "0123456789ABCDEF"[hashCode[i] & 0xf];
  }
  return Password;
}

/*
 * @brief:Aliyun_IoT 接入函数
 * @param: productKey 产品Key 
 * @param: deviceName 设备名
 * @param: deviceSecret 设备密码
 * @retval: none
*/
void AliyunIoT(String productKey, String deviceName, String deviceSecret)
{
  String signcontent;
  /*拼接服务器地址(只针对华东2,其它地区需要修改cn-shanghai)*/
  aliyun_mqtt_server = productKey + ".iot-as-mqtt.cn-shanghai.aliyuncs.com";
  /*拼接MQTT客户端ID(只采用TCP直连和hmacsha256加密方式)*/
  aliyun_clientID = deviceName + "|securemode=3,signmethod=hmacsha256|";
  /*拼接MQTT客户端名*/
  aliyun_clientName = deviceName + "&" + productKey;
  /*拼接MQTT客户端密码加密对象*/
  signcontent = "clientId" + deviceName + "deviceName" + deviceName + "productKey" + productKey;
  /*获取MQTT客户端密码*/
  aliyun_clientPwd = hmac256(signcontent, deviceSecret);
  /*设置mqtt服务器地址和端口*/
  client.setServer(aliyun_mqtt_server.c_str(), 1883);
  /*设置mqtt保持活跃时间 ms*/
  client.setKeepAlive(60);
  /*设置消息回调函数*/
  client.setCallback(callback);

  while (!client.connect(aliyun_clientID.c_str(), aliyun_clientName.c_str(), aliyun_clientPwd.c_str()))
  {
    Serial.println("Aliyun_IoT is connecting ...");
    delay(1000);
  }
  Serial.println("Aliyun_IoT is connected");

  String AttributesSet = "/sys/" + productKey + "/" + deviceName + "/thing/service/property/set";
  client.subscribe(AttributesSet.c_str(), 0);
}
/*====================================== Aliyun_IOT =========================================*/
#else
/*
 * @brief:MQTT连接
 * @param:none
 * @retval:none
*/
void mqtt_connect()
{
  /*设置mqtt服务器地址和端口*/
  client.setServer(mqtt_server, 1883);
  /*设置mqtt保持活跃时间 ms*/
  client.setKeepAlive(60);
  /*设置消息回调函数*/
  client.setCallback(callback);

  /**
   * 连接服务器 
   * 
   * client.connect(ClientID,Name,Password)
   * 
   * ClientID : 接入服务器的MQTT客户端ID
   * Name ：接入服务器的用户
   * Password ：接入服务器的密码
   * 
   */
  while (!client.connect(clientID, clientName, clientPwd))
  {
    Serial.println("MQTT is connecting ...");
    delay(1000);
  }
  Serial.println("MQTT is connected");
  /**
   * 订阅主题
   * 
   * client.subscribe(topic,qos)
   * 
   * topic ： 订阅的主题
   * qos : 服务质量,Broker 与 Client 之间消息通信关系. 0 or 1 or 2
   * 最多一次（0）只发不管收没收到
   * 最少一次（1）至少接受到一次
   * 只一次  （2）确保只接受一次,慢,安全最高  
   * 
   */
  client.subscribe("/LED_ON", 0);
  client.subscribe("/LED_OFF", 0);
  client.subscribe("/test", 0);
  /**
   * 发出消息
   * 
   * client.publish(topic,payload,retained)
   * 
   * topic ： 发布的主题
   * payload ： 消息体
   * retained ：保留消息 true or false
   */
  client.publish("/test", "... this is a test of mqtt ...", false);
}
#endif
/*====================================== 初始化 =========================================*/
/*
 * @brief:初始化 
 * @param:none
 * @retval:none
*/
void setup()
{
  Serial.begin(115200); //设置串口波特率
  /*====================用户初始化=======================*/
  /**如 IO口模式初始化**/
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);

  /*====================用户初始化=======================*/
  wifi_connect(); //wifi连接
#ifdef Aliyun_IoT
  AliyunIoT(productKey, deviceName, deviceSecret); //Aliyun 连接
#else
  mqtt_connect();   //mqtt连接
#endif
}

/*===================================== 循环执行 ========================================*/
/*
 * @brief:需要循环执行的代码 
 * @param:none
 * @retval:none
*/
void loop()
{
  /**判断 WIFI & MQTT 连接状态**/
  while (!WiFi.isConnected() || !client.connected())
  {
    wifi_connect(); //wifi连接
#ifdef Aliyun_IoT
    AliyunIoT(productKey, deviceName, deviceSecret); //Aliyun 连接
#else
    mqtt_connect(); //mqtt连接
#endif
  }
  client.loop();

  /*==================== 用户使用=======================*/
  /**如 需要不断地向云上传数据**/
  //String data = "{\"Temperature\":"+temp+","+"\"Humidity\":"+humi+"}";//{"Temperature":20,"Humidity":20}
  //client.publish(xxtopic,data.c_str());
  /*==================== 用户使用=======================*/
  delay(500);
}
