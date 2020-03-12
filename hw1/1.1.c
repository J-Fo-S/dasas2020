//
// ffmpeg-pipe approach taken from: https://batchloaf.wordpress.com/2017/02/10/a-simple-way-to-read-and-write-audio-and-video-files-in-c-using-ffmpeg/
// Homework 1a
// To compile:
//
//    gcc 1a.c -o 1a -lm
//
 
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
 
#define N 16000
 
void main()
{
    // get working directory for pipeout - NOTE: not necessary after all, but leaving
    //char* cwd;
    //char cwd_b[1024];
    //char s_out_b[1024];
    //cwd = getcwd(cwd_b, sizeof(cwd_b));
    
    // Create audio buffer
    int16_t buf[N] = {0}; // buffer
    double G[N] = {0}; // g func 
    double Y[N] = {0}; // y func 
    int n;                // buffer index
    double fs = 16000.0;  // sampling frequency
    double t = 1.0 / fs;
    double fo = 1000.0;
    int m = N / 5;
    //double tv = (N-1)*t;  // time vector 

     
    // Generate 1 second of audio data
    // TODO: fix bug on G indexing (printf G[n])
    for (n=0 ; n<N ; ++n) 
    {
        if (n-(N/2) <= (int)m && n-(N/2) >= (int)-m) 
            {
                G[n] = cos((M_PI/2.0)*((n-(N/2))/(double)m));
                G[n] = G[n]*G[n];
                //G[n] = 1.0;
            }
        else 
            {
                G[n] = 0;
            }
        // center G over middle of file
        Y[n] = 0.5 * G[n] * sin(2.0*M_PI*fo*n*t);
        buf[n] = Y[n] * 16383.0;
    }
     
    // Pipe the audio data to ffmpeg, which writes it to a wav file in cwd
    FILE *pipeout;
    //pipeout = popen(strcat(strcat(strcpy(s_out_b, "ffmpeg -y -f s16le -ar 16000 -ac 1 -i - "), cwd), "/1a.wav"), "w");
    pipeout = popen("ffmpeg -y -f s16le -ar 16000 -ac 1 -i - 1a.wav", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
}