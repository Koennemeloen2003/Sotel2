/* SOTELI GAME proto.1 demo and test firmware
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio.h"
#include "spi.h"

#include "esp_random.h"
#include "esp_timer.h"
#include "esp_event.h"

#include "wifi.h"
#include "udploop.h"

#include "nvs_flash.h"

static struct spi spi = {};
static struct udploop udploop = {};

// Kleur-macro,  overgenomen uit display.h (van de originele versie):
#define COLOR(R, G, B) (((R & 0x1f) << 11) | ((G & 0x3f) <<  5) | (B & 0x1f))

void app_sotelii(void)
{
    if (!wifi_init())
    {
        printf("unable to initialize Wi-Fi\n");

        return;
    }


    if (!udploop_init(&udploop))
    {
        printf("unable to initialize UDP receiver loop\n");

        return;
    }


    if (!gpio_init())
    {
        printf("unable to initialize gpio's\n");

        return;
    }


    if (!spi_init(&spi))
    {
        printf("unable to initialize spi\n");

        return;
    }


 /* De functie `esp_timer_get_time` geeft de verstreken tijd terug sinds de
    laatste start (of reset) van de ESP32, in µs. Je kunt met deze functie een
    “benchmark” uitvoeren (van een blok code): als je in het begin (van een blok
    code) deze tijd opvraagt en dit op het einde (van een blok code) nogmaals
    doet dan zal het verschil tussen deze twee tijden de tijd (in µs) zijn die
    het codeblok heeft gekost. */

    uint64_t tijd_begin = 0, tijd_nu = esp_timer_get_time();

    int tijd_stopOntvangen = 0, tijd_delta = 0;
    unsigned int ticks = 0;

    unsigned char wifi_ontvangen = 0, wifi_teller = 0;

   int test =0;

    while (1) // ================= MAIN  LOOP =================
    {
        tijd_begin = esp_timer_get_time();

     // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (!(ticks %100)){
            test +=1;
        }
        if (test >= 0x10){
            test = 0;
        }
        ticks++;
        
        if (!(ticks % 64))
           // wifi_send_if_associated(COLOR(0x1f, 0, 0), ticks, 0);

        if (wifi_ontvangen > 0)
        {
            wifi_teller++;

         /* Hier kun je de ontvangen Wi-Fi gegevens verwerken. Dit doe je door
            de waarde `wifi_ontvangen` te gebruiken als het aantal te verwerken
            bytes. De te verwerken bytes staan in `udploop.buffer`. */
            if (udploop.buffer[0] == 'R'){
                spi_transfer(&spi, 'R');
                sys_delay_ms(100);
                int response2 = spi_transfer(&spi, 0x01);
                printf(" Res: %02x", response2);
                wifi_send_if_associated(COLOR(0x1f, 0, 0), response2, 0);

            }
            else{
            for(int i =0; i< wifi_ontvangen;i++){
                printf(" %02x", udploop.buffer[i]);
                int response1 = spi_transfer(&spi, udploop.buffer[i]);
                if (response1 == -1)
                    {
                    printf("spi is gestopt met functioneren\n");

                     return;
                     }
            }
            }
            printf("\n");
            wifi_ontvangen = 0;
            
        }
       /*
       int spi_ontvangen = spi_transfer(&spi, test);
        
        if (spi_ontvangen == -1)
        {
            printf("spi is gestopt met functioneren\n");

            return;
        }
        */
        //printf(" %02x", spi_ontvangen);
        if (!(ticks % 16))
            printf(" %x\n (%02x) ", test, wifi_teller);

     // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        tijd_nu = esp_timer_get_time();

     /* In het hierboven met ‘stippel-lijnen‘ omgeven codeblok kan je het beste
        allerlei functionaliteit implementeren. De code beneden moet alleen
        het ontvangen van Wi-Fi gegevens afhandelen terwijl het verwerken van
        die ontvangen gegevens dus in het met ‘stippel-lijnen‘ omgeven codeblok
        moet gebeuren: dan is de kans het grootst dat het codeblok op strict
        periodieke wijze zal lopen. */

        tijd_stopOntvangen = tijd_nu + 50000 - (tijd_nu - tijd_begin);

        while ( (tijd_nu = esp_timer_get_time()) < tijd_stopOntvangen )
        {
            tijd_delta = tijd_stopOntvangen - tijd_nu;

            int action = udploop_wait(&udploop, tijd_delta - 10000);

            if (action == -1)
                return;

            if (action == 1)
            {
                wifi_ontvangen = udploop_receive(&udploop);
            }
        }
    }
    
}


void app_main(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());

        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    app_sotelii();
}
