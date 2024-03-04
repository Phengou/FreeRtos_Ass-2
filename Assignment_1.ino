//Name CHEA Phengou
//ID: 6672054521

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>
#include <NTPClient.h>
#include <FreeRTOS.h>
#include <esp_wifi.h>
#include <task.h>
#include <esp_sleep.h>
#include <esp_task_wdt.h>

// Wi-Fi and NTP Configuration
const char* ssid = "IOTs";
const char* password = "24681012";
const char* ntpServer = "th.pool.ntp.org";
const long gmtOffset = 7 * 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset);

// Task Handles
TaskHandle_t Thread1;
TaskHandle_t Thread2;


// Task Functions
void task1(void* pvParameters) {
  for (;;) {
    time_t now = time(NULL);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    Serial.print("Thread 1 - Bangkok Time: "); 
    Serial.println(asctime(&timeinfo));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void task2(void* pvParameters) {
  for (;;) {
    timeClient.update();
    time_t ntpTime = timeClient.getEpochTime();
    struct timeval tv;
    tv.tv_sec = ntpTime;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
    Serial.println("Thread 2 - Got NTP server updated:");
    vTaskDelay(pdMS_TO_TICKS(60000));
  }
}

// Setup Function
void setup() {
  Serial.begin(115200);

  // Initialize Watchdog Timer
  esp_task_wdt_init(5, true); // 5 seconds timeout, panic handler enabled


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Initialize NTP client
  timeClient.begin();

  // Create and start tasks
  xTaskCreatePinnedToCore(
    task1,
    "Thread 1",
    10000,
    NULL,
    1,
    &Thread1,
    0
  );

  xTaskCreatePinnedToCore(
    task2,
    "Thread 2",
    10000,
    NULL,
    2,
    &Thread2,
    0
  );

  // Start FreeRTOS scheduler
  vTaskStartScheduler();
}

// Loop Function (Empty)
void loop() {
  // Empty loop
}
