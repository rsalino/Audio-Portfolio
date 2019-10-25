//
//  biquad.h
//  
//
//  Created by Rob Salino on 10/21/19.
//

#ifndef biquad_h
#define biquad_h


#endif /* biquad_h */


#include <stdlib.h>
#include <math.h>
#include "common/multicore_shared_memory.h"
#include "common/audio_system_config.h"

#define PI 3.14159265358979232846
#define FLT_MIN_PLUS 1.175494351e-38
#define FLT_MIN_MINUS -1.175494351e-38


class Biquad{
protected:
    float Xz_1, Xz_2,Yz_1,Yz_2;
public:
    float a0, a1, a2, b1, b2;
    float gain, note;
    bool playing;
    void reset()
    {
        Xz_1 = Xz_2 = Yz_1 = Yz_2 = gain = note = 0;
        playing = false;
    }
    void set(float fc, float g, float Q)
    {
        float K = tan(PI * fc/AUDIO_SAMPLE_RATE);
        float delta = pow(K,2) * Q + K + Q;
        a0 = K/delta;
        a1 = 0.0;
        a2 = -a0;
        b1 = (2*Q*(pow(K,2)-1))/delta;
        b2 = (pow(K,2)*Q - K + Q)/delta;
        playing = true;
        note = fc;
        gain = g;
    }
    float doBiquad(float xn)
    {
        float yn = a0*xn + a1*Xz_1 + a2*Xz_2 - b1*Yz_1 - b2*Yz_2;
        Xz_2 = Xz_1;
        Xz_1 = xn;
        Yz_2 = Yz_1;
        Yz_1 = yn;
        if(yn > FLT_MIN_MINUS && yn < 0)
        {
            yn = 0;
        }
        else if(yn < FLT_MIN_PLUS && yn > 0)
        {
            yn = 0;
        }
        return yn*gain;
    }
};

