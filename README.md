# Audio-Portfolio
Various audio-related programs

## port7.c
Uses port audio and libsndfile to enable recording in a basic terminal program
Must have libsndfile and PortAudio installed before use
Compile: cc -o portrecord Port_Record.c -lsndfile -lportaudio

## 4_Channel_Swapping.c
Enables the user to split multi-channel audio files into separate mono files
Compile: gcc -o downmixing downmixing.c -lsndfile

## Decay_Curves.c
Creation of envelopes for various synthesizer waveforms

## Sawtooth_Generator.c
Creation of sawtooth synthesizer waveform at user-specified length, frequency, and up/down shape
