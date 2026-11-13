#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"


const uint8_t servo_pin1 = 17;
const uint8_t servo_pin2 = 16;
const uint8_t servo_pin3 = 2;
const uint8_t servo_pin4 = 4;
const uint8_t pwm_freq = 50;
const uint16_t pwm_period_us = 20000;
const int pwm_res = LEDC_TIMER_14_BIT;

const uint8_t stepper_pins[] = {18, 19, 21, 22}; 

void servo_set_angle(int angle) {
    int pulse = 500 + angle * 2000 / 180;
    int duty = pulse * 16384 / pwm_period_us;

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    printf("Servo angle: %d, pulse: %d", angle, pulse);
    fflush(stdout);
}

void step() {
    gpio_set_level(stepper_pins[0], 1);
    gpio_set_level(stepper_pins[1], 0);
    gpio_set_level(stepper_pins[2], 0);
    gpio_set_level(stepper_pins[3], 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(stepper_pins[0], 0);
    gpio_set_level(stepper_pins[1], 1);
    gpio_set_level(stepper_pins[2], 1);
    gpio_set_level(stepper_pins[3], 0);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void app_main(void) {
    printf("Started\n");
    fflush(stdout);

    vTaskDelay(pdMS_TO_TICKS(2000));

    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = pwm_res,
        .freq_hz = pwm_freq,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    ledc_channel_config_t channel = {
        .gpio_num = servo_pin1,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    channel.gpio_num = servo_pin1;
    ESP_ERROR_CHECK(ledc_channel_config(&channel));

    printf("PWM initialised");
    fflush(stdout);

    gpio_reset_pin(stepper_pins[0]);
    gpio_reset_pin(stepper_pins[1]);
    gpio_reset_pin(stepper_pins[2]);
    gpio_reset_pin(stepper_pins[3]);

    gpio_set_direction(stepper_pins[0], GPIO_MODE_OUTPUT);
    gpio_set_direction(stepper_pins[1], GPIO_MODE_OUTPUT);
    gpio_set_direction(stepper_pins[2], GPIO_MODE_OUTPUT);
    gpio_set_direction(stepper_pins[3], GPIO_MODE_OUTPUT);

    while(1) {
        channel.gpio_num = servo_pin1;
        ESP_ERROR_CHECK(ledc_channel_config(&channel));
        servo_set_angle(0);
        vTaskDelay(pdMS_TO_TICKS(20));

        channel.gpio_num = servo_pin2;
        ESP_ERROR_CHECK(ledc_channel_config(&channel));
        servo_set_angle(0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        channel.gpio_num = servo_pin1;
        ESP_ERROR_CHECK(ledc_channel_config(&channel));
        servo_set_angle(90);
        vTaskDelay(pdMS_TO_TICKS(20));

        channel.gpio_num = servo_pin2;
        ESP_ERROR_CHECK(ledc_channel_config(&channel));
        servo_set_angle(90);
        vTaskDelay(pdMS_TO_TICKS(1000));

        channel.gpio_num = servo_pin1;
        ESP_ERROR_CHECK(ledc_channel_config(&channel));
        servo_set_angle(0);
        vTaskDelay(pdMS_TO_TICKS(20));

        channel.gpio_num = servo_pin2;
        ESP_ERROR_CHECK(ledc_channel_config(&channel));
        servo_set_angle(0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        // step();
    }
}