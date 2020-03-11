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
#include <stdlib.h>

// sig length as exponential of 2 (for ease)
#define N 16384 // num samples
#define m_fft 256 // fft window size
#define NUM_COMMANDS 9 // for gnuplot pipe command
 
void main()
{    
    //double P[N];           // power spectrum of x
    int i;                 // indexer
    int j;                 // indexer
    int n;                // wav buffer index
    int k;                // freq buffer index
    double m = N / 2;        // envelope window length
    double fs = 16000.0;  // sampling frequency
    double t = 1.0 / fs;  // sampling period
    double fo = 1000.0;   // pip freqency
    double f;             // freq band for STFT
    // Create audio buffer
    //int16_t buf[N] = {0}; // buffer
    double G[N] = {0}; // g hann env window func
    double G_fft[m_fft] = {0}; // g hann fft window func 
    double Y[N] = {0}; // y func
    int h_t = (int)(N/m_fft); // # of fft frames
    double H[N/m_fft][m_fft] = {{0}}; // h func
    double X_r[N/m_fft][m_fft] = {{0}}; // x real
    double X_i[N/m_fft][m_fft] = {{0}}; // x imag
    //double tv = (N-1)*t;  // time vector 
     
    // Generate 1 second of audio data
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
    //printf("%lf \n", Y[16383]);
    
    // Transform to STFT
    // time (frames) index
    for (i=0; i<h_t; ++i)
    {
        // frequencies index (bins)
        for (k=0; k<m_fft; ++k)
        {
            f = (k/(float)m_fft)*fs; // freq bands across domain
                // iterate through m_fft window
                for (j=0; j<m_fft; ++j)
                {
                    // hann window each fft window
                    if (j-(m_fft/2) <= m_fft/2 && j-(m_fft/2) >= -m_fft/2) 
                    {
                        //if (j-(m_fft/2) < 0) break;
                        G_fft[j] = cos((M_PI/2.0)*((j-(m_fft/2))/((double)m_fft/2.0)));
                        G_fft[j] = G_fft[j]*G_fft[j];
                        //printf("%f \n", G_fft[j]);
                        //if (k == 1) exit(0);
                    }
                    else 
                    {
                        G_fft[j] = 0;
                    }

                    X_r[i][k] += Y[i * m_fft+j] * G[j] * cos((j * f * 2.0 * M_PI) / m_fft); // not sure about j term (i.e. relationship to n in DFT)
                    X_i[i][k] -= Y[i * m_fft+j] * G[j] * sin((j * f * 2.0 * M_PI) / m_fft);
                    //printf("%lf \n", Y[i*m_fft+j]);
                    //if (X_i[i][k] > 0) printf("%f", X_i[i][k]);
                }
            H[i][k] = X_r[i][k] + X_i[i][k];
            //printf("%lf %lf \n", H[i][k], f);    
            //P[i][k] = H[i][k] * H[i][k]; // power spectrum
        }
    } 
   
    // pipe method of Gnuplot
    char * commandsForGnuplot[] = {"set terminal svg", 
    "set output 'stft1000.svg'",
    "set title \"STFT\"", 
    "set style line 11 lc rgb '#808080' lt 1",
    "set border 3 back ls 11",
    "set size ratio 1.4",
    "set xrange [1:63]",
    "set yrange [1:8000]",
    "plot 'stft1000.txt' u 1:($2-65*$1):3 with image title 'stft spectrogram'"};
    FILE * temp = fopen("stft1000.txt", "w");

    double mag_spec2[(int)(N/m_fft)][m_fft] = {{0}};
    // take double-sided spectrogram
    for (i=0; i < h_t; i++)
    {
        for (j=0; j < m_fft; j++)
        {
            //f = (j/(float)m_fft)*fs; // freq domain
            mag_spec2[i][j] = fabs(H[i][j]/m_fft); // double-sided spectrogram NOTE: mag not factoring in window effect on magnitude?
        }
    }

    double mag_spec1[(int)(N/m_fft)][m_fft/2] = {{0}};
    //double graph_spec1[(int)(N/m_fft)][(int)(N/m_fft)][m_fft/2] = {{{0}}};
    // take single-sided from double-sided spectrogram
    for (i=0; i < h_t; i++)
    {
        for (j=0; j < m_fft/2; j++)
        {
            f = (j/(float)m_fft)*fs;
            mag_spec1[i][j] = mag_spec2[i][j+1];
            if (j > 1) mag_spec1[i][j] = 2*mag_spec1[i][j]; // compensate for neg amplitudes?
            if (j == m_fft/2-1) mag_spec1[i][j] = 0;
            printf("%lf %lf \n", mag_spec1[i][j], f);
            fprintf(temp, "%i %lf %lf \n", i, f, mag_spec1[i][j]); //Write the data to a temporary file
        }
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
    //fclose(stft1000.txt);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    //remove("stft1000.txt");

}