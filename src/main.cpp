/**
 * VÍ DỤ 3-03: ĐA NHIỆM VỚI FREERTOS SCHEDULER & MUTEX
 */
#include <Arduino.h>
#include <PTITCube.h>

PTIT_CDH cdh;
PTIT_Scheduler scheduler;

#define LED_PIN 2

void heartbeatTask(void* pvParameters) {
    pinMode(LED_PIN, OUTPUT);
    bool state = false;
    for (;;) {
        state = !state;
        digitalWrite(LED_PIN, state);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void telemetryTask(void* pvParameters) {
    uint16_t id = 0;
    for (;;) {
        float voltage = 3.7f + (float)random(-20, 20) / 100.0f;
        float temp    = 24.5f + (float)random(-10, 30) / 10.0f;

        if (scheduler.lock(PTIT_BUS_SERIAL, 100)) {
            cdh.printf("TELEM: #%04u | V: %.2fV | T: %.1fC | Uptime: %lus\n",
                       id++, voltage, temp, millis() / 1000);
            scheduler.unlock(PTIT_BUS_SERIAL);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void diagnosticTask(void* pvParameters) {
    for (;;) {
        if (scheduler.lock(PTIT_BUS_SERIAL, 100)) {
            cdh.printf("DIAG: Free Heap: %u bytes | CPU: %u MHz\n",
                       ESP.getFreeHeap(), ESP.getCpuFreqMHz());
            scheduler.unlock(PTIT_BUS_SERIAL);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void setup() {
    cdh.begin();

    scheduler.addTask("Heartbeat",  heartbeatTask,  500,  1, 1);
    scheduler.addTask("Telemetry",  telemetryTask,  1000, 1, 1);
    scheduler.addTask("Diagnostic", diagnosticTask, 5000, 0, 0);

    scheduler.start();
}

void loop() {
}
