#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>
#include <stdatomic.h>

#include "SeaBreezeWrapper.h"
#include "util.h"

#include "control.h"
#include "spectrometer.h"
#include "queue.h"

void* spectrometer(void *p)
{
    struct controlQueue *spectrometer_control = (struct controlQueue*)p;
    struct LamportQueue *queue = spectrometer_control->queue;
    struct control *c = spectrometer_control->control;
    
    struct spectral to_push;

    int error = 0;
    double spectrum[PIXELS] = {0};
    
    struct timespec now;

    if( *c->RUN_SPECTROMETER ) open_spectrometer();

    while( ! *c->STOP ){


        usleep(S_TIME);

        if( *c->RUN_SPECTROMETER ){

            seabreeze_set_integration_time_microsec(0, &error, *c->exposure);
            if (error) perror("exposure not set");
            to_push.exposure = *c->exposure;

            clock_gettime(CLOCK_REALTIME, &now);
            to_push.time = (now.tv_sec - *c->start_time) * 1000 + (now.tv_nsec) / 1.0e6 ;

            seabreeze_get_formatted_spectrum(0, &error, spectrum, sizeof(spectrum));
            if (error) perror("no spectrum");

            if( *c->OUTPUT ){

                for(int i=0; i<PIXELS; i+=100 ){
                    printf("%05f ", spectrum[i] );
                }
                printf("\n");
            }
        }
        else{
            clock_gettime(CLOCK_REALTIME, &now);
            to_push.time = (now.tv_sec - *c->start_time) * 1000 + (now.tv_nsec) / 1.0e6 ;
            usleep( *c->exposure );
        }
        memcpy(to_push.spectrum, spectrum, sizeof(spectrum));

        if( *c->DEBUG ) printf("pushing spectra \n");

        if(LamportQueue_push(queue, (void*)&to_push) ) // all's well
            ;
        else if( *c->DEBUG ) printf( "spectrometer queue full\n");

    }

    if( *c->RUN_SPECTROMETER ){
         seabreeze_close_spectrometer(0, &error);        // zero = only spectrometer
        if (error) perror("spectrometer unclosed");

    }
   
    if(*c->DEBUG) printf("spectrometer finished\n");

    while( ! *c->FINISHED ) // wait up
      ;
    return 0;
}

void auto_correct( double *white_bal, struct control *c )
{
    int error = 0;
    bool discontent = true;
    int max_val;
    int clip_count;
    char buffer[50];

    double spectrum[PIXELS];
    open_spectrometer();

    do{
        max_val = 0;
        clip_count = 0;
        seabreeze_set_integration_time_microsec(0, &error, *c->exposure);
        if (error) perror("time not set");

        seabreeze_get_formatted_spectrum(0, &error, spectrum, sizeof(spectrum));
        if (error) perror("no spectrum");

        for(int i=0; i<PIXELS; i++){
            if(spectrum[i] > max_val)
                max_val = spectrum[i];
            if(spectrum[i] == 16383 )  // 2^14 -1 (the fully exposed pixel)
                clip_count++;
        }
        printf("The white balance peaked at\n"
               "intensity value: %d\n", max_val);
        if(clip_count)
            printf("A total of %d pixels were overexposed\n", clip_count);
        else printf("...or %f%% of maximum.\n", (float)max_val/16383);

        printf("The exposure was set to %d\n", *c->exposure);
        printf("If this is acceptable, enter K\n");
        
        fgets( buffer, 50, stdin );
        if(buffer[0]=='K'){
            memcpy(white_bal, spectrum, sizeof(spectrum));
            printf("\nGreat, initialized\n");
            usleep(200000);
            discontent = false;
        }
        else{
            printf("Please enter a new exposure:\n");
            fgets( buffer, 50, stdin );
            *c->exposure = atoi( buffer );
            printf("Attempting exposure %d\n\n", *c->exposure);
        }

    }while(discontent);

    seabreeze_close_spectrometer(0, &error);
    if (error) perror("spectrometer unclosed");

}

void open_spectrometer()
{
    int error = 0;

    seabreeze_open_spectrometer(0, &error); // zero = only spectrometer
    if (error){
        perror("spectrometer didn't open");
        exit(0);
    }
    unsigned test_pixels = seabreeze_get_formatted_spectrum_length(0, &error);
    if (error){
        perror("no spectrum length");
        exit(0);
    }
    if( test_pixels != PIXELS ){
        perror("not right number of pixels");
        exit(0);
    }
}

void get_wavelengths(struct control *c)      // unused, save once
{
    int error = 0;
    double wavelengths[PIXELS];
    open_spectrometer();

    seabreeze_set_integration_time_microsec(0, &error, *c->exposure);
    if (error) perror("time not set");

    seabreeze_get_wavelengths(0, &error, wavelengths, sizeof(wavelengths)); // unused
    if (error){
        perror("no wavelengths fetched");
        exit(0);
    }
    for(int i=0; i<PIXELS; i+=100 ){
        printf("%05f ", wavelengths[i] );
    }
    printf("\n");

    seabreeze_close_spectrometer(0, &error);        // zero = only spectrometer
    if (error) perror("spectrometer unclosed");

}

