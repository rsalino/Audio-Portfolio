/* Taken from:
 downmixing.c: downmix a stereo file into a mono file
 Support input file with the following extensions: .wav, .aif, .aiff
 Copyright (C) 2019 Ming-Lun Lee
 with recommended revisions by Steve Philbert
 <minglunlee@rochester.edu>
 Channel Swapping and format conversion edits by Rob Salino
 Compile: gcc -o downmixing downmixing.c -lsndfile
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sndfile.h>  //THIS IS FROM libsndfile

#define NFRAMES (1024)  // block size: number of frames per block

void print_sfinfo(const SF_INFO * sfinfo); // print sfinfo to the console
int  sf_extension(const char * filename);  // determine the major format with the file extension (e.g. wav, aif, aiff).

enum {ARG_PROGNAME, ARG_INFILE, ARG_OUTFILE, ARG_NARGS}; // for command line arguments

int main (int argc, char * argv [])
{
    char * infilename;         // input file name
    char * outfilename;        // output file name
    SNDFILE * infile = NULL ;  // input sound file pointer
    SNDFILE * outfile = NULL;  // output sound file pointer
    SF_INFO   sfinfo;          // sound file info
    SF_FORMAT_INFO info;       // sound file format info
    int outfile_major_type;    // output major type in hex
    long nsamples;             // number of samples per block
    long readcount;            // no. of samples read
    float * buffer = NULL;     // buffer pointer (a dynamic array of floats)
    int i;
    float channel1, channel2, channel3, channel4, temp;
    
    if(argc != ARG_NARGS){
        printf("Swapping channels in a four-channel file:\n2(Channel 1 <-> Channel 4)\n(Channel 2 <-> Channel 3)\nInput file should have four channels.\n");
        printf("insufficient arguments.\n");
        printf("Usage: %s infile outfile\n", argv[ARG_PROGNAME]);
        return 1;
    }
    
    infilename = argv[ARG_INFILE] ;    // infile name
    outfilename = argv[ARG_OUTFILE] ;  // outfile name
    
    // input validation
    if (strcmp (infilename, outfilename) == 0)
    {   printf ("Error : Input and output filenames are the same.\n") ;
        return 1 ;
    } ;
    
    memset(&sfinfo, 0, sizeof (sfinfo));  // clear sfinfo
    
    /* Open input sound file  for reading &
     fill sound file information with sfinfo. */
    if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
    {
        printf ("Not able to open input file %s.\n", infilename) ;
        puts (sf_strerror (NULL)) ;
        return 1 ;
    } ;
    if(sfinfo.channels != 4)
    {
        printf("Error: input file doesn't have four channels.");
        return 1;
    }
    
 
    printf("sample rate = %d\n", sfinfo.samplerate);
    printf("number of channels = %d\n", sfinfo.channels);
    
    printf("Input file info:\n");
    print_sfinfo(&sfinfo); // print input file format information

    nsamples = sfinfo.channels * NFRAMES; // no. of samples per block
    buffer = (float *)malloc(nsamples * sizeof(float)); // used to save a block of samples
    
    outfile_major_type = sf_extension(outfilename); // return outfile major type in hex
    if(outfile_major_type == -1){
        printf("The outfile extension is not .wav, .aif, or .aiff\n");
        return 1;
    }

    memset(&sfinfo, 0, sizeof (sfinfo));  // clear sfinfo

       if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
       {    printf ("Not able to open input file %s.\n", infilename) ;
           puts (sf_strerror (NULL)) ;
           return 1 ;
       } ;
    
    sfinfo.channels = 4;
    if (!sf_format_check(&sfinfo))
    {
        sf_close(infile) ;
        printf ("Invalid encoding\n") ;
        return 1;
    }
    sf_command(NULL, SFC_GET_FORMAT_MAJOR, &info, sizeof(info)) ;
    sfinfo.format = SF_FORMAT_FLOAT | info.format;
    // open a sound file for writing with sfinfo
    if((outfile = sf_open(outfilename, SFM_WRITE, &sfinfo)) == NULL)
    {
        printf("Not able to open output file %s.\n", outfilename) ;
        puts(sf_strerror (NULL)) ;
        return 1 ;
    };
    printf("Output file info:\n");
    print_sfinfo(&sfinfo); // print output file format information
    
    while ((readcount = sf_read_float(infile, buffer, nsamples)))
    {
        //channel 1(buffer[i]) <-> channel 4(buffer[i+3]); channel 2(buffer[i+1] <-> channel 3(buffer[i+2])
        for(i = 0; i < readcount; i += 4) //temp swap the channels:
        {
            temp = buffer[i];
            buffer[i] = buffer[i + 3];
            buffer[i + 3] = temp;
            temp = buffer[i + 1];
            buffer[i + 1] = buffer[i + 2];
            buffer[i + 2] = temp;
        }
        sf_write_float(outfile, buffer, (int)(readcount));
    }    // write block by block until the end of the sound file

    printf("Conversion is done!\n");
    
    sf_close (infile) ;   // close input sound file
    sf_close (outfile) ;  // close output text file
    
    return 0 ;
}

/*
print_sfinfo() is used to print the sound file information
*/

void print_sfinfo(const SF_INFO * sfinfo){
    int majortype = sfinfo->format & SF_FORMAT_TYPEMASK; // find the major type
    int subtype = sfinfo->format & SF_FORMAT_SUBMASK;    // find the subtype
    int major_count;  // number of major types
    int sub_count;    // number of subtypes
    SF_FORMAT_INFO formatTemp;
    memset(&formatTemp, 0, sizeof(formatTemp));   // clear the SF_FORMWT_INFO struct
    
    // full format    
    /*** major type ***/
    sf_command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &major_count,sizeof(int));
    for(int i=0; i<major_count; i++) {
        formatTemp.format = i;
        sf_command (NULL, SFC_GET_FORMAT_MAJOR, &formatTemp, sizeof(formatTemp));
        if (majortype == formatTemp.format) {
            printf(" %02d 0x%08x %s (extension \"%s\")\n", i, formatTemp.format, formatTemp.name, formatTemp.extension);
            break; // find the matching format
        }
    }
    
    /*** subtype ***/
    memset(&formatTemp, 0, sizeof(formatTemp));
    sf_command(NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &sub_count,sizeof(int));
    for(int i=0; i<sub_count; i++) {
        formatTemp.format = i;
        sf_command (NULL, SFC_GET_FORMAT_SUBTYPE, &formatTemp, sizeof(formatTemp));
        if (subtype == formatTemp.format) {
            printf(" %02d 0x%08x %s\n", i, formatTemp.format, formatTemp.name);
            // extension is not available for subtypes
            break; //find the matching format
        }
    }
}

/*
 Determine the file major type with the file extension (e.g. wav, aif, aiff).
 Return the major type format in hex.
 */
int sf_extension(const char * filename){
    int filename_len = strlen(filename); // entire filename length (include the extension)
    if(strcmp((filename + (filename_len - 4)), ".wav") == 0){
        return SF_FORMAT_WAV;     // wav file type in hex
    }
    else if(strcmp((filename + (filename_len - 4)), ".aif") == 0
        || strcmp((filename + (filename_len - 5)), ".aiff") == 0){
        return SF_FORMAT_AIFF;   // aiff or aif file type in hex
    }
    else {
        return -1;               // extension is not wav, aiff, or aif
    }
}
