#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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
  display.print("TempInterna");
  display.print(": ");
  display.print(temperaturaInterna);
  display.print(" C");
  display.display();

  Serial.println("imprimeTemperaturaInterna");
}

void imprimeTemperaturaAgua(float temperataruaAgua)
{
  display.fillRect(0, 10, SCREEN_WIDTH, 10, SSD1306_BLACK); // Limpa a segunda linha
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10); // Segunda linha
  display.print("TempAgua");
  display.print(": ");
  display.print(temperataruaAgua);
  display.print(" C");
  display.display();

  Serial.println("imprimeTemperaturaAgua");
}

void imprimeTemperaturaAmbiente(float temperaturaAmbiente)
{
  display.fillRect(0, 20, SCREEN_WIDTH, 10, SSD1306_BLACK); // Limpa a terceira linha
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20); // Terceira linha
  display.print("TempAmb");
  display.print(1);
  display.print(": ");
  display.print(temperaturaAmbiente);
  display.print(" C");
  display.display();

  Serial.println("imprimeTemperaturaAmbiente");
}

void imprimeHumidadeAmbiente(float humidade)
{
  display.fillRect(0, 30, SCREEN_WIDTH, 10, SSD1306_BLACK); // Limpa a terceira linha
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30); // quarta linha
  display.print("Humidade");
  display.print(1);
  display.print(": ");
  display.print(humidade);
  display.print(" %");
  display.display();

  Serial.println("imprimeHumidadeAmbiente");
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
  }
  else {
    imprimeTemperaturaAmbiente(event.temperature);
  }

  Serial.println("temperaturaDht");
}

void humidadeDht()
{
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    imprimeHumidadeAmbiente(0);
  }
  else {
    imprimeHumidadeAmbiente(event.relative_humidity);
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
        }
        else if(i == 1)
        {
          imprimeTemperaturaAgua(temp);
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
}

void loop() {
  Serial.println("Iniciando LOOP...");
  leituraTemperatura();
  humidadeDht();
  temperaturaDht();
  delay(4000);
}