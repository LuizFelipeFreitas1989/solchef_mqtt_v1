#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <AdafruitIO_WiFi.h>

#define DHTPIN 4
#define DHTTYPE    DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
sensor_t sensor;
sensors_event_t event;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
const int PINO_SCL = 22;
const int PINO_SDA = 21;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int PINO_ONEWIRE = 18;
OneWire oneWire(PINO_ONEWIRE);
DallasTemperature sensors(&oneWire);
DeviceAddress ds18;
byte c;
float lastTemp = 0.0;
float temp = 0.0;

const char* ssid = "TP-Link_54F7";
const char* password = "08350954";

// configuração mqtt
#define IO_USERNAME  "lf_felipefreitas1989"
#define IO_KEY       "aio_ZZTf20Av0C2ep9JFoMsJK13Y6wE2"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, ssid, password);
AdafruitIO_Feed *feedTemperaturaInterna = io.feed("TemperaturaInterna");
AdafruitIO_Feed *feedTemperaturaAgua = io.feed("TemperaturaAgua");
AdafruitIO_Feed *feedTemperaturaAmbiente = io.feed("TemperaturaAmbiente");
AdafruitIO_Feed *feedHumidadeAmbiente = io.feed("HumidadeAmbiente");

void iniciaDisplay()
{
  Wire.begin(PINO_SDA, PINO_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();
  Serial.print("Display inicializado...");
}

void imprimeTemperaturaInterna(float temperaturaInterna)
{
  display.fillRect(0, 0, SCREEN_WIDTH, 10, SSD1306_BLACK); // Limpa a primeira linha
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.print("Temp. Int");
  display.print(": ");
  display.print(temperaturaInterna);
  display.print(" C");
  display.display();

  Serial.println("imprimeTemperaturaInterna");
}

void imprimeTemperaturaAgua(float temperataruaAgua)
{
  display.fillRect(0, 15, SCREEN_WIDTH, 10, SSD1306_BLACK); // Limpa a segunda linha
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 15); // Segunda linha
  display.print("Temp. Agua");
  display.print(": ");
  display.print(temperataruaAgua);
  display.print(" C");
  display.display();

  Serial.println("imprimeTemperaturaAgua");
}

void imprimeTemperaturaAmbiente(float temperaturaAmbiente)
{
  display.fillRect(0, 30, SCREEN_WIDTH, 10, SSD1306_BLACK); // Limpa a terceira linha
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30); // Terceira linha
  display.print("Temp. Ext");
  display.print(": ");
  display.print(temperaturaAmbiente);
  display.print(" C");
  display.display();

  Serial.println("imprimeTemperaturaAmbiente");
}

void imprimeHumidadeAmbiente(float humidade)
{
  display.fillRect(0, 45, SCREEN_WIDTH, 10, SSD1306_BLACK); // Limpa a terceira linha
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 45); // quarta linha
  display.print("Humidade");
  display.print(": ");
  display.print(humidade);
  display.print(" %");
  display.display();

  Serial.println("imprimeHumidadeAmbiente");
}

void iniciaWifi()
{
  WiFi.begin(ssid, password);
  // Aguarde conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }

  Serial.println("Conectado ao Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void conectaBroker(){
  //mensagem inicial
  Serial.print("Conectando ao Adafruit IO");
  // chama função de conexão io.adafruit.com
  io.connect();
  // Aguarda conexação ser estabelecida
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Conectado
  Serial.println();
  Serial.println(io.statusText());
}

void iniciatDht()
{
  dht.begin();
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  Serial.println("inicio DHT");
}

void temperaturaDht()
{
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    imprimeTemperaturaAmbiente(0);
    feedTemperaturaAmbiente -> save(0);
  }
  else {
    imprimeTemperaturaAmbiente(event.temperature);
    feedTemperaturaAmbiente -> save(event.temperature);
  }

  Serial.println("temperaturaDht");
}

void humidadeDht()
{
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    imprimeHumidadeAmbiente(0);
    feedHumidadeAmbiente -> save(0);
  }
  else {
    imprimeHumidadeAmbiente(event.relative_humidity);
    feedHumidadeAmbiente -> save(event.relative_humidity);
  }

  Serial.println("humidadeDht");
}

void iniciaTemperatura()
{
  sensors.begin();
  c = sensors.getDeviceCount();
  Serial.print("Sensores encontrados: ");
  Serial.println(c);
}

void leituraTemperatura()
{
  sensors.requestTemperatures();
  for(int i = 0; i < c; i++)
  { 
    if(sensors.getAddress(ds18, i))
    {
       temp = sensors.getTempC(ds18);

        if(abs(temp - lastTemp) > 1)
        {
          lastTemp = temp;
        }
        if(i == 0)
        {
          imprimeTemperaturaInterna(temp);
          feedTemperaturaInterna -> save(temp);
        }
        else if(i == 1)
        {
          imprimeTemperaturaAgua(temp);
          feedTemperaturaAgua -> save(temp);
        }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando SETUP...");
  iniciaTemperatura();
  iniciaDisplay();
  iniciatDht();
  iniciaWifi();
  conectaBroker();
}

void loop() {
  Serial.println("Iniciando LOOP...");
  // processa as mensagens e mantêm a conexão ativa
  byte state = io.run();

  //verifica se está conectado
  if(state == AIO_NET_DISCONNECTED | state == AIO_DISCONNECTED){
    conectaBroker(); //função para conectar ao broker
  }

  leituraTemperatura();
  humidadeDht();
  temperaturaDht();
  delay(10000);

  display.clearDisplay();
  display.display();
  delay(5000);
}