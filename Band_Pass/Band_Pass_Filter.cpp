//
//  Band_Pass_Filter.cpp
//  
//
//  Created by Rob Salino on 10/21/19.
//  Preconfigured setup by Analog Devices, Inc. for use with the SHARC Audio Module (much of it is not used to implement the header file Band_Pass_Header.h
//  ***Due to copyright, only including the customized section that uses the header file biquad.h, which holds the info to create the filter***

#include <audio_processing/audio_effects_selector.h>
#include <math.h>
#include "biquad.h"
Biquad filter;

// Define your audio system parameters in this file
#include "common/audio_system_config.h"

// Support for simple multi-core data sharing
#include "common/multicore_shared_memory.h"

// Variables related to the audio framework that is currently selected (e.g. input and output buffers)
#include "audio_framework_selector.h"

// Includes all header files for effects and calls for effect selector
#include "audio_processing/audio_effects_selector.h"

// Prototypes for this file
#include "callback_audio_processing.h"


void processaudio_setup(void)
{
 filter.reset();
}

#pragma optimize_for_speed
void processaudio_callback(void)
{
    float freq = 3450*(multicore_data->audioproj_fin_pot_hadc0) + 50;
    float qual = 10*(multicore_data->audioproj_fin_pot_hadc1);
    float gain = multicore_data->audioproj_fin_pot_hadc2;

    filter.set(freq, gain, qual);

    for (int i = 0; i < AUDIO_BLOCK_SIZE; i++)
    {


        audiochannel_0_left_out[i] = filter.doBiquad(audiochannel_0_left_in[i]);
        audiochannel_0_right_out[i] = filter.doBiquad(audiochannel_0_right_in[i]);
    }

}
