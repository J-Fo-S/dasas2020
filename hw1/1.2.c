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

// sig length 
#define N 16000
#define NUM_COMMANDS 7
 
void main()
{    
    // Create audio buffer
    //int16_t buf[N] = {0}; // buffer
    double G[N] = {0}; // g func 
    double Y[N] = {0}; // y func
    double H[N] = {0}; // h func
    double X_r[N] = {0}; // x real
    double X_i[N] = {0}; // x imag
    //double P[N];           // power spectrum of x
    int i;                 // indexer
    int n;                // wav buffer index
    int k;                // freq buffer index
    double m = N / 2;        // window length
    double fs = 16000.0;  // sampling frequency
    double t = 1.0 / fs;  // sampling period
    double fo = 1000.0;   // pip freqency
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

        //buf[n] = Y[n] * 16383.0;
    }
    
    // Transform to DFT and power spectrum // NOTE: could sum here to N/2 and compute only single-side spectrogram
    for (k=0 ; k<N ; ++k) 
    {
        for (n=0 ; n<N ; ++n) 
        {
            X_r[k] += Y[n] * cos((n * k * 2.0 * M_PI) / N);
            X_i[k] -= Y[n] * sin((n * k * 2.0 * M_PI) / N);
        }
        H[k] = X_r[k] + X_i[k];
        //P[k] = H[k] * H[k]; // power spectrum
    }

    // Pipe the audio data to ffmpeg, which writes it to a wav file in cwd
    //FILE *pipeout;
    //pipeout = popen("ffmpeg -y -f s16le -ar 16000 -ac 1 -i - 2a_sin.wav", "w");
    //fwrite(buf, 2, N, pipeout);
    //pclose(pipeout);

    // pipe method of Gnuplot
    char * commandsForGnuplot[] = {"set terminal svg", 
    "set output 'dft1000.svg'", 
    "set title \"DFT\"",
    "set xlabel 'freq HZ'",
    "set ylabel 'normalized magnitude'", 
    "set key inside left top vertical Right noreverse enhanced autotitles box linetype -1 linewidth 1.000",
    "plot 'data.temp' every 2::0 ls 1 w lines title 'mag spectrogram'"};
    FILE * temp = fopen("data.temp", "w");
    //FILE * temp2 = fopen("data2.temp", "w");

    // take single-sided spectrogram
    //double f;
    //double mag_spec;
    //for (k=0; k < N/2; k++)
    //{
    //    f = (k/(float)N)*fs; // freq domain
    //    mag_spec = 2*fabs(H[k+2]/(N/2)); // double-sided spectrogram
    //    fprintf(temp, "%lf %lf \n", f, mag_spec); //Write the data to a temporary file
    //    //fprintf(temp2, "%lf %lf \n", xvals[i], zvals[i]); //Write the data to a temporary file
    //}

    // following matlab example https://www.mathworks.com/help/matlab/ref/fft.html
    double f;
    double mag_spec2[N] = {0};
    // take double-sided spectrogram
    for (i=0; i < N; i++)
    {
        f = (i/(float)N)*fs; // freq domain
        mag_spec2[i] = fabs(H[i]/N); // double-sided spectrogram NOTE: mag not factoring in window effect on magnitude?
        //fprintf(temp, "%lf %lf \n", f, mag_spec2); //Write the data to a temporary file
    }
    double mag_spec1[N/2] = {0};
    // take single-sided from double-sided spectrogram
    for (i=0; i < N/2+1; i++)
    {
        f = (i/(float)N)*fs;
        mag_spec1[i] = mag_spec2[i+1];
        if (i > 1) mag_spec1[i] = 2*mag_spec1[i]; // compensate for neg amplitudes?
        if (i == N/2-1) mag_spec1[i] = 0;
        fprintf(temp, "%lf %lf \n", f, mag_spec1[i]); //Write the data to a temporary file
    }

        /*Opens an interface that one can use to send commands as if they were typing into the
     *     gnuplot command line.  "The -persistent" keeps the plot open even after your
     *     C program terminates.
     */
    FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    for (i=0; i < NUM_COMMANDS; ++i)
    {
        fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
    }
    pclose(gnuplotPipe);
    //fclose(temp);
    //fclose(temp2);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    remove("data.temp");
    //remove("data2.temp");
}