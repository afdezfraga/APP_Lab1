/* Dot product of two vectors */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(argc,argv)
int argc;
char *argv[];
{
    double start, end;
    int N=100000000, i;
    float *x, *y;
    float dot;
    
    if (argc < 2) {
     	fprintf(stderr,"Use: %s num_elem_vector\n", argv[0]);
     	exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);

      /* Allocate memory for vectors */
    if((x = (float *) malloc(N*sizeof(float))) == NULL)
        printf("Error in malloc x[%d]\n",N);
    if((y = (float *) malloc(N*sizeof(float))) == NULL)
        printf("Error in malloc y[%d]\n",N);

      /* Inicialization of x and y vectors*/
    for(i=0; i<N; i++){
	x[i] = (N/2.0 - i);
	y[i] = 0.0001*i;
    }

    /* Dot product operation */

    start = omp_get_wtime();

    dot = 0.;
    #pragma omp parallel for reduction(+:dot) private(i) schedule(static,4)
    for(i=0; i<N; i++)
	dot += x[i] * y[i];

    end = omp_get_wtime();
  
    printf("Dot product = %g\n", dot);
    printf("Execution time = %g secondos\n ", end-start);

    return 0;
}



