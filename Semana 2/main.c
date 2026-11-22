#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define LED_PIN 2
#define PUERTO UART_NUM_0
#define BUF_SIZE 256

int comparar(char cmd[], char ord1[], char ord2[], char ord3[]){
    if (strcmp(cmd, ord1) == 0){
        return 1;
    }
    else if (strcmp(cmd, ord2) == 0){
        return 2;
    }
    else if (strcmp(cmd, ord3) == 0){
        return 3;
    }
    else{
        return 0;
    }
}

// Manda un texto al monitor por el puerto
void enviar(const char *texto){
    uart_write_bytes(PUERTO, texto, strlen(texto));
}

// Prepara el puerto serie (igual que el ejemplo uart_echo)
void iniciar_uart(void){
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(PUERTO, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(PUERTO, &config);
    uart_set_pin(PUERTO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// Espera una palabra y Enter. Devuelve 1 si cupo, 0 si era demasiado larga.
int leer_palabra(char destino[], int max){
    int largo = 0;
    bool demasiado_larga = false;

    while(1){
        uint8_t c;
        int n = uart_read_bytes(PUERTO, &c, 1, pdMS_TO_TICKS(100));

        if(n <= 0){
            continue;          // no llegó nada en 100 ms: sigue esperando
        }

        if(c == '\n' || c == '\r'){
            if(largo == 0){
                continue;      // Enter sin palabra
            }
            enviar("\r\n");
            break;
        }

        uart_write_bytes(PUERTO, (const char *)&c, 1);   // eco de la letra

        if(largo < max - 1){
            destino[largo] = (char)c;
            largo++;
        }
        else{
            demasiado_larga = true;
        }
    }

    destino[largo] = '\0';

    if(demasiado_larga){
        return 0;
    }
    return 1;
}

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    iniciar_uart();

    char comando[20];
    char orden1[] = "prender";
    char orden2[] = "apagar";
    char orden3[] = "estado";
    bool estado = false;
    int contador = 0;

    enviar("\r\n Escribe 'prender', 'apagar' o 'estado':\r\n");

    while(1){
        int cabe = leer_palabra(comando, 20);

        if(cabe == 0){
            enviar("Orden no reconocida\r\n");
            continue;
        }

        int res = comparar(comando, orden1, orden2, orden3);

        if(res == 1){
            enviar("LED encendido!!\r\n");
            gpio_set_level(LED_PIN, 1);
            estado = true;
            contador++;
            printf("Contador= %d\r\n", contador);
        }
        else if(res == 2){
            enviar("LED apagado!!\r\n");
            gpio_set_level(LED_PIN, 0);
            estado = false;
            contador++;
            printf("Contador= %d\r\n", contador);
        }
        else if(res == 3){
            if (estado == true){
                enviar("Estado: Encendido\r\n");
            }
            else{
                enviar("Estado: Apagado\r\n");
            }
            printf("Contador= %d\r\n", contador);
        }
        else{
            enviar("Orden no reconocida\r\n");
        }
    }
}