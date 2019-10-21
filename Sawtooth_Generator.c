//
//  Sawtooth_Generator.c
//
//
//  Created by Rob Salino on 9/30/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum {arg_name, arg_sawtype, arg_min, arg_max, arg_freq, arg_duration, arg_samplerate, arg_output, args}; //argument names for all of the command line items

int main(int argc, char * argv[])
{
    double sawtype, min, max, freq, duration, samplerate, sample; //to convert the entered values from command line to doubles (done below)
    double increment; //my inverse "run" value for each wave (in rise/run)
    double slope; //to switch between - and + rise
    double intercept; //to switch between intercept for up/down saw
    int check1, check2, check3, check4, check5, check6, check7; //to check if certain conditions are met when typing in values for each command line arguments
    int i, sample_count; //for iterating through the entire sound file duration
    short samp; //for writing shorts to output file
    FILE* fo;
    fo = fopen(argv[arg_output],"wb"); //output file
    //below is changing all entered values in command line to floating point values
    sawtype = atof(argv[arg_sawtype]);
    min = atof(argv[arg_min]);
    max = atof(argv[arg_max]);
    freq = atof(argv[arg_freq]);
    duration = atof(argv[arg_duration]);
    samplerate = atof(argv[arg_samplerate]);
    
    sample_count = (int)(duration * samplerate); //calculating the total number of samples to be written
    increment = freq/samplerate; //inverse of the "run" (to be multiplied by rise)
    
    if(argc != args) //if number of arguments entered isn't accurate, gives how to use program
    {
        printf("** Sawtooth Wave Generator (Output to a 32-bit float raw audio file) **\nUsage: /hw4_1 sawtype min_amplitude max_amplitude frequency duration sample_rate output_rawfile\nsawtype: 0 for upward sawtooth, 1 for downward sawtooth\nmin_amplitude, max_amplitude: amplitude boundary between -1 and 1.\nfrequency: in Hz\nduration: in seconds\nsample_rate: e.g. 44100\noutput_rawfile: raw file name, e.g. output.raw\n");
        return 1;
    }
    //initializing checks to zero for input validation
    check1 = 0;
    check2 = 0;
    check3 = 0;
    check4 = 0;
    check5 = 0;
    check6 = 0;
    check7 = 0;
    
    if(sawtype == 0 || sawtype == 1) //input validation: saw type entered must be 0 or 1 (for upward and downward saw, respectively)
    {
        check1 = 1;
    }
    else
    {
        printf("Error: Saw type must be 0 for upward sawtooth or 1 for downward sawtooth.\n");
    }
    if(min >= -1 && min <= 1) //input validation: min must be between -1 and 1
    {
        check2 = 1;
    }
    else
    {
        printf("Error: min_amplitude must be between -1.0 and 1.0.\n");
    }
    if(min < max) //input validation: min must be less than max
    {
        check3 = 1;
    }
    else
    {
        printf("Error: min_amplitude must be less than max_amplitude, e.g. -0.5, 0.9.\n");
    }
    if(max >= -1 && max <= 1) //input validation: max must be between -1 and 1
    {
        check4 = 1;
    }
    else
    {
        printf("Error: max_amplitude must be between -1.0 and 1.0.\n");
    }
    if(freq > 0) //input validation: frequency must be positive value
    {
        check5 = 1;
    }
    else
    {
        printf("Error: frequency must be positive.\n");
    }
    if(duration > 0) //input validation: duration must be positive value
    {
        check6 = 1;
    }
    else
    {
        printf("Error: duration must be positive.\n");
    }
    if(samplerate > 0) //input validation: sample rate must be positive value
    {
        check7 = 1;
    }
    else
    {
        printf("Error: samplerate must be positive.\n");
    }
    if(sawtype == 1) //downward saw
    {
        slope = -(max-min); //negative slope
        intercept = max;
    }
    else if(sawtype == 0) //upward saw
    {
        slope = (max-min); //positive slope
        intercept = min;
    }
    
    if(check1 == 1 && check2 == 1 && check3 == 1 && check4 == 1 && check5 == 1 && check6 == 1 && check7 == 1) //input validation: if all validations pass...
    {
        for(i = 0; i < sample_count; i++) //iterate until total number of samples
        {
            sample = slope * increment * fmod(i,1/increment) + intercept; //sample for down saw is -(rise) * (run inverse) * (remainder of floating point division of index / (run inverse) + max value (y-intercept)
                /* use 32767 to avoid overflow problem */
            samp = (short) (sample * 32767.0);
            if(fwrite(&samp,sizeof(short),1,fo) != 1) //write to output file check for error
            {
                printf("Error writing data to file\n");
                return 1;
            }
        }
    }
    else //if input validation fails
    {
        printf("Please run the program again with correct command line arguments.\n");
        return 0;
    }
}

