/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/
pthread_t *thr;
sem_t *sem;
int NTHREADS=3;
/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
        /*
         * x and y traverse the complex plane.
         */
        double x, y;

        int n;
        int val;

        /* Find out the y value corresponding to this line */
        y = ymax - ystep * line;

        /* and iterate for all points on this line */
        for (x = xmin, n = 0; n < x_chars; x+= xstep, n++) {

                /* Compute the point's color value */
                val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
                if (val > 255)
                        val = 255;

                /* And store it in the color_val[] array */
                val = xterm_color(val);
                color_val[n] = val;
        }
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
        int i;

        char point ='@';
        char newline='\n';

        for (i = 0; i < x_chars; i++) {
                /* Set the current color, then output the point */
                set_xterm_color(fd, color_val[i]);
                if (write(fd, &point, 1) != 1) {
                        perror("compute_and_output_mandel_line: write point");
                        exit(1);
                }
        }

        /* Now that the line is done, output a newline character */
        if (write(fd, &newline, 1) != 1) {
                perror("compute_and_output_mandel_line: write newline");
                exit(1);
        }
}

void compute_and_output_mandel_line(int fd, int line)
{
        /*
         * A temporary array, used to hold color values for the line being drawn
         */
        int color_val[x_chars];

        compute_mandel_line(line, color_val);
        sem_wait(&sem[line % NTHREADS]);
        output_mandel_line(fd, color_val);
        sem_post(&sem[(line + 1) % NTHREADS]);
}

void *thread_function(void* line_arg){
        int*ptr = (int*)line_arg;
        int line;
        for (line = *ptr; line < y_chars; line += NTHREADS) {
                compute_and_output_mandel_line(1, line);
        }
        pthread_exit(0);
}

void *my_malloc(size_t size){
        void* ptr;
        if((ptr=malloc(size))==NULL){
                printf("out of memory");
                exit(1);
        }
        return ptr;
}

int main(int argc, char *argv[])
{

        if(argc!=2){
                printf("Usage : ./mandle number_of_threads\n");
                exit(1);
        }
        NTHREADS=atoi(argv[1]);

        if(NTHREADS > y_chars){
                 NTHREADS = y_chars;
                 fprintf(stderr, "Threads has changed to %d\n", NTHREADS);
        }

        printf("Running for %d threads",NTHREADS);

        sem=my_malloc(NTHREADS *sizeof(sem_t));


        sem_init(&sem[0], 0, 1);
        int i=0;
        for ( i = 1; i < NTHREADS; i++) {
                sem_init(&sem[i], 0, 0);
                    }

        int line;

        xstep = (xmax - xmin) / x_chars;
        ystep = (ymax - ymin) / y_chars;

        pthread_t *threads =my_malloc(NTHREADS * sizeof(pthread_t));
        int *args =my_malloc(NTHREADS * sizeof(int));

        for (line = 0; line < NTHREADS; line++) {
                args[line] = line;
                pthread_create(&threads[line], NULL, thread_function, &args[line]);
        }

        for (line = 0; line < NTHREADS; line++) {
                pthread_join(threads[line], NULL);
        }
        reset_xterm_color(1);
        return 0;
        }
