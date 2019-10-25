//
//  Decay_Curves.c
//  
//
//  Created by Rob Salino on 9/23/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI (3.141592654)

enum {arg_name,arg_output,arg_amplitude,arg_freq,arg_duration,arg_samplerate,arg_envtype,args};
char* envtype[4] = {"none","lin","log","sqr"};

int main(int argc, char * argv[])
{
    int i, sample_count;
    double start, end, amplitude, freq, duration, samplerate, step, angleincr, sample;
    double factor_lin, factor_log, factor_sqr;
    double twopi = 2.0 * PI;
    double j;
    FILE* fo = NULL;
    float fsamp;
    short samp;
    fo = fopen(argv[arg_output],"wb");
    
    amplitude = atof(argv[arg_amplitude]);
    freq = atof(argv[arg_freq]);
    duration = atof(argv[arg_duration]);
    samplerate = atof(argv[arg_samplerate]);

    if(argc != args)
    {
        printf("Usage: /hw3_2 outfile amp freq dur srate envtype\nenvtype: none, lin, log, or sqr");
        return 1;
    }
    
    sample_count = (int)(duration * samplerate);
    angleincr = (twopi * freq) / samplerate;
    step = duration / sample_count;

    if(strcmp(argv[6],envtype[0]) == 0)
    {
        for(i = 0; i < sample_count; i++)
        {
            sample = amplitude * sin(angleincr*i);
            /* use 32767 to avoid overflow problem */
            samp = (short) (sample * 32767.0);
            if(fwrite(&samp,sizeof(short),1,fo) != 1)
            {
                printf("Error writing data to file\n");
                return 1;
            }
        }
    }
    else if(strcmp(argv[6],envtype[1]) == 0)
    {
        for(i = 0; i < sample_count; i++)
        {
            factor_lin = amplitude - (amplitude * i/sample_count);
            sample = factor_lin * sin(angleincr*i);
            /* use 32767 to avoid overflow problem */
            samp = (short) (sample * 32767.0);
            if(fwrite(&samp,sizeof(short),1,fo) != 1)
            {
                printf("Error writing data to file\n");
                return 1;
            }
        }
    }
    else if(strcmp(argv[6],envtype[2]) == 0)
    {
        for(i = 0; i < sample_count; i++)
        {
            factor_log = amplitude - log10((10 * amplitude * i/sample_count) + 1);
            if(factor_log < 0)
            {
                factor_log = 0;
            }
            sample = factor_log * sin(angleincr*i);
            /* use 32767 to avoid overflow problem */
            samp = (short) (sample * 32767.0);
            if(fwrite(&samp,sizeof(short),1,fo) != 1)
            {
                printf("Error writing data to file\n");
                return 1;
            }
        }
    }
    else if(strcmp(argv[6],envtype[3]) == 0)
    {
        for(i=0;i < sample_count; i++)
        {
            factor_sqr = amplitude - amplitude * i/sample_count * i/sample_count;
            sample = factor_sqr * sin(angleincr*i);
            /* use 32767 to avoid overflow problem */
            samp = (short) (sample * 32767.0);
            if(fwrite(&samp,sizeof(short),1,fo) != 1)
            {
                printf("Error writing data to file\n");
                return 1;
            }
        }
    }
    else
    {
        printf("Not sure what you tried to do there... Try again\n");
        return 0;
    }
    fclose(fo);
}
