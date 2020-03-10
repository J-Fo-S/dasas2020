#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define NUM_POINTS 500
#define NUM_COMMANDS 7

int main()
{
    char * commandsForGnuplot[] = {"set terminal svg", "set output 'simple.1pipe.svg'", 
    "set title \"TITLEEEEE\"", "set key inside left top vertical Right noreverse enhanced autotitles box linetype -1 linewidth 1.000",
    "set style line 1 lt 0 lc rgb 'red' lw 3", "set style line 2 lt 0 lc rgb 'green' lw 3",
    "plot 'data.temp' ls 1 title 'ls 1', 'data2.temp' ls 2 title 'ls 2'"};
    double xvals[NUM_POINTS] = {0};
    double yvals[NUM_POINTS] = {0};
    double zvals[NUM_POINTS] = {0};
    FILE * temp = fopen("data.temp", "w");
    FILE * temp2 = fopen("data2.temp", "w");
    /*Opens an interface that one can use to send commands as if they were typing into the
     *     gnuplot command line.  "The -persistent" keeps the plot open even after your
     *     C program terminates.
     */
    FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    int i;
    for (i=0; i < NUM_POINTS; i++)
    {
        xvals[i] = sin(M_PI*i/NUM_POINTS);
        yvals[i] = atan(M_PI*i/NUM_POINTS);
        zvals[i] = cos(atan(M_PI*i/NUM_POINTS));
        fprintf(temp, "%lf %lf \n", xvals[i], yvals[i]); //Write the data to a temporary file
        fprintf(temp2, "%lf %lf \n", xvals[i], zvals[i]); //Write the data to a temporary file
    }

    for (i=0; i < NUM_COMMANDS; i++)
    {
        fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
    }
    pclose(gnuplotPipe);
    //fclose(temp);
    //fclose(temp2);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    remove("data.temp");
    remove("data2.temp");

    return 0;
}