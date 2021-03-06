# APP_Lab1

## Vectorización

### Ejercicio 1: multf
-----

#### Modificaciones en el código

El código consta de un triple bucle, donde las iteraciones tienen la misma carga de trabajo. Debido a esto se paralelizará el bucle más externo, dividiendo sus iteraciones entre los hilos. Además se añadirá vectorización, incluyendo la cláusula `#pragma omp simd` en el bucle más interno.

De esta forma el triple bucle quedaría de la siguiente manera:

```c
#pragma omp parallel for private(i,j,k,temp) schedule(static)
for(i=0;i< NRA;i++)
    for(j=0;j< NCB;j++)
    {
        temp = 0.0;
        #pragma omp simd reduction(+:temp)
        for(k=0;k< NCA;k++)
            temp += a[i][k] * b[j][k];
        d[i][j] = temp;
    }
```

#### Evaluación del rendimiento


| .....          | 1 Hilo    | 2 Hilos     | 4 Hilos     |
| :-------------:|----------:| -----------:| -----------:|
| Sin vectorizar | 7.783 s   | 3.914 s     | 2.013 s     |
| Vectorizado    | 4.264 s   | 2.150 s     | 1.077 s     |

El programa escala de forma cercana a lo ideal al aumentar el número de hilos. Además de esto, al añadir vectorización para cualquier cantidad de hilos se obtiene una aceleración de x1.75 ~ x2 respecto a la versión sin vectorizar.

### Ejercicio 2: saxpy
-----

#### Modificaciones en el código

Se han modificado los bucles indicados para permitir vectorización añadiendo la cláusula `#pragma omp simd`.

```c
for (k = 0; k < NREPS;k++) {
    #pragma omp simd
    for(j= 0; j< N; j++){
        saxpy(a, b, j, 4.0f);
    }
}

.....

for (k = 0; k < NREPS;k++) {
    #pragma omp simd
    for(j= 0; j< N; j++){
        b[j] = saxpyi(a[j], b[j], 4.0f);
    }
}
```

Además sobre las cabeceras de las funciones indicadas se ha añadido la cláusula `#pragma omp declase simd` para permitir su vectorización. 

```c
#pragma omp declare simd notinbranch aligned(X,Y:32) uniform(X,Y,a) linear(i:1)
void saxpy ( float *X, float *Y, int i, float a)

.....

#pragma omp declare simd notinbranch
float saxpyi( float x, float y, float a)
```

#### Evaluación del rendimiento


| Saxpy          | FT2       | Local     |
| :-------------:|----------:|----------:|
| Sin vectorizar | 1.275 s   | 1.007 s   |
| Vectorizado    | 1.109 s   | 0.623 s   |

| Saxpyi         | FT2       | Local     |
| :-------------:|----------:|----------:|
| Sin vectorizar | 1.269 s   | 1.034 s   |
| Vectorizado    | 1.126 s   | 0.621 s   | 

Vemos que existe una ligerísima mejora al introducir vectorización en el Finisterrae, sin embargo la mejora es mucho más notable en otros sistemas, como mi ordenador local, que obtiene aceleraciones similares al Finisterrae en el resto de situaciones.

(1) Procesador de mi ordenador local: Intel(R) Core(TM) i7-10870H CPU @ 2.20GHz.

### Ejercicio 3: saxpy
-----

#### Modificaciones en el código

En los cuatro bucles del código añadimos la directiva `#pragma omp parallel for` para paralelizar dichos bucles.

#### Evaluación del rendimiento

| Saxpy          | 1 Hilo    | 2 Hilos     | 4 Hilos     |
| :-------------:|----------:| -----------:| -----------:|
| Sin vectorizar | 1.323 s   | 0.926 s     | 0.585 s     |
| Vectorizado    | 1.158 s   | 0.659 s     | 0.517 s     |

| Saxpyi         | 1 Hilo    | 2 Hilos     | 4 Hilos     |
| :-------------:|----------:| -----------:| -----------:|
| Sin vectorizar | 1.305 s   | 0.658 s     | 0.346 s     |
| Vectorizado    | 1.158 s   | 0.572 s     | 0.303 s     |

Observamos que al aumentar la cantidad de hilos la versión 'saxpyi' escala mejor que la versión 'saxpy'. Además notamos que la vectorización sigue añadiendo una mejora muy ligera al rendimiento del programa en todos los casos, aunque cada vez más diluida al reducirse los tiempos de cómputo a menos de 1 segundo.

### Ejercicio 4: Jacobi
-----

#### Modificaciones en el código

En la fúnción `jacobi` existen dos dobles bucles paralelizados, uno de 'inicialización de datos' y otro más principal.

En ambos casos el bucle externo está paralelizado con un `pragma omp parallel for`, en concreto:

```c
/* Copy new solution into old*/
#pragma omp parallel for private(i,j)
for(i=0;i<l;i++)
    for(j=0;j<t;j++)
        ...

/* Compute stencil, residual, & update*/
#pragma omp parallel for private(i,j,resid) reduction(+:error)
for(i=1;i<l-1;i++)
    for(j=1;j<t-1;j++)
        ...
```

Lo principal a tener en cuenta es que en el doble bucle principal, la variable `error` es una variable de reducción. Quitando esto, la vectorización en completamente directa, añadiendo la cláusula `pragma omp simd` al bucle interno. El resultado quedaría de la siguiente forma:

```c
/* Copy new solution into old*/
#pragma omp parallel for private(i,j)
for(i=0;i<l;i++)
    #pragma omp simd
    for(j=0;j<t;j++)
        ...

/* Compute stencil, residual, & update*/
#pragma omp parallel for private(i,j,resid) reduction(+:error)
for(i=1;i<l-1;i++)
    #pragma omp simd reduction(+:error)
    for(j=1;j<t-1;j++)
        ...
```

#### Evaluación del rendimiento


| .....          | 1 Hilo    | 2 Hilos     | 4 Hilos     | 8 Hilos     | 16 Hilos     |
| :-------------:|----------:| -----------:| -----------:| -----------:| ------------:|
| Sin vectorizar | 31.564 s  | 15.847 s    | 8.161 s     | 4.512 s     | 2.491 s      |
| Vectorizado    | 18.672 s  | 9.412 s     | 4.822 s     | 2.705 s     | 1.518 s      |

Notamos que ambas versiones del programa escalan de forma cercana a lo ideal. Además, observamos que el rendimiento mejora con la vectorización, en concreto se puede apreciar una mejora en el rendimiento de aproximadamente x1.75.

### Ejercicio 5: Swim
-----

#### Modificaciones en el código

Se han modificado los bucles en las funciones calc1, calc2 y calc3.

La mayoría de bucles se pueden vectorizar sin mayor problema, sin embargo el doble bucle principal de las funciones calc1 y calc2 no es tan simple. Existen dependencias entre las iteraciones y openmp no es capaz de autovectorizar estos bucles.

Por lo tanto, en estos bucles aplicamos loop fision, para separar las partes vectorizables, de las partes con dependencias. Tras esto hicimos una pequeña evaluación experimental que demostró que en determinados casos, se perdía más con el loop fision de lo que se ganaba con la vectorización. Tras estos pasos llegamos a la versión final de la herramienta.

Cabe destacar que en los bucles vectorizados en los que fuese necesario se cambió la política de `static` a `simd:static`.

#### Evaluación del rendimiento

La evaluación de rendimiento se ha realizado usando el fichero `swim.in.ref`.

| .....          | 1 Hilo    | 2 Hilos     | 4 Hilos     |
| :-------------:|----------:| -----------:| -----------:|
| Sin vectorizar | 21.925 s  | 10.875 s    | 5.700 s     |
| Vectorizado    | 17.736 s  | 9.221 s     | 4.991 s     |

En este caso podemos ver que los beneficios de la vectorización, pese a ser significativos, son mucho menores que en otros casos, ya que los bucles computacionalmente más costosos solo se pueden vectorizar parcialmente, por lo que la mejora introducida por la vectorización es menos notable. La aceleración que se consigue oscila entre el 20% y el 15% y tiende a descender según aumenta el número de hilos.

## Afinidad de hilos

### Ejercicio 1: dotprod
-----

#### Modificaciones en el código

Se ha añadido la cláusula `#pragma omp parallel for` al bucle principal para paralelizar el código.

#### Evaluación del rendimiento

| .....        | 12 Hilos - OMP_PLACES=sockets |
| :-----------:|------------------------------:|
| Master       | 1.921 s                       |
| Spread       | 8.487 s                       |

Este programa tiene muy poca complejidad aritmética, es decir, es claramente memory bound. Por eso, la gestión de la jerarquía de memoria es muy importante para obtener un buen rendimiento. Los datos son inicializados por completo por el hilo principal, por lo que están en su memoria. Usando la política `master` los hilos se ejecutan en el mismo lugar que el hilo principal y por tanto esa memoria está cerca de ellos. Usando la política `spread`, los hilos se separan entre los sockets, lo que causa más latencia en los accesos a memoria empeorando el rendimiento del programa.

### Ejercicio 2: multf
-----

#### Modificaciones en el código

No se ha modificado el código respecto al ejercicio anterior.

#### Evaluación del rendimiento

| .....        | 12 Hilos - OMP_PLACES=sockets |
| :-----------:|------------------------------:|
| Master       | 0.404 s                       |
| Close        | 0.402 s                       |

En este programa no se aprecian diferencias al cambiar la política. En este programa la complejidad aritmética es mayor, y ese es el cuello de botella por encima de los accesos a memoría. Además usando 12 hilos en ambas políticas hay recursos hardware para dar cabida a todos los hilos, es decir, si se usasen 16 hilos, sí veríamos una mejora de la política `close` respecto a la `master`, por poder dar cabida también a estos 4 nuevos hilos.

## Programación híbrida

### Ejercicio 1: pi_integral
-----

#### Modificaciones en el código

En primer lugar distribuimos la cantidad de intervalos que vamos a calcular usando un `MPI_Bcast()`.

```c
MPI_Bcast( &n , 1 , MPI_INT , MPI_ROOT_PROCESS , MPI_COMM_WORLD);
```

A continuación, cada proceso calcula, cuáles de esos intervalos debe calcular.

```c
    n_local = n / npes;
    n_resto = n % npes;

    // For [myid*n_local, (myid+1*n_local)-1]
    // Do calculations
    if (myid < n_resto){
        start_local = myid * (n_local + 1); 
        end_local = (myid + 1) * (n_local + 1);
    } else {
        start_local = (myid * n_local) + n_resto;
        end_local = ((myid + 1) * n_local) + n_resto;
    }
```

Cada proceso calcula esos intervalos de forma concurrente, usando un `#pragma omp parallel for`.

```c
    // pi_local = h * sum
    #pragma omp parallel for private(x) reduction(+:sum)
    for (i = start_local; i < end_local; i ++) {
        x = h * ((double)i + 0.5);   //height of the rectangle
        sum += 4.0 / (1.0 + x*x);
    }

    pi_local = h * sum;
```

Finalmente, los procesos juntan sus resultados parciales en el proceso 0 usando `MPI_Reduce()`.

```c
MPI_Reduce( &pi_local, &pi, 1, MPI_DOUBLE, MPI_SUM, MPI_ROOT_PROCESS, MPI_COMM_WORLD);
```

#### Evaluación del rendimiento


| .....          | 1 Proc - 1 Hilo   | 1 Proc - 16 Hilos | 2 Procs - 8 Hilos | 4 Procs - 4 Hilos | 8 Procs - 2 Hilos | 16 Procs - 1 Hilo |
| :-------------:| -----------------:| -----------------:| -----------------:| -----------------:| -----------------:| -----------------:|
| pi_integral    | 4.263 s  (x1)     | 0.540 s  (x7.89)  | 0.475 s  (x8.97)  | 0.304 s  (x14.04) | 0.307 s  (x13.89) | 0.303 s  (x14.07) |

Podemos observar que el programa funciona mejor con más hilos y menos procesos, en concreto el programa alcanza su máximo rendimiento al usar 4 hilos por proceso o menos. El rendimiento empeora al usar 2 procesos y 8 hilos por proceso y empeora incluso más al usar un solo hilo y 16 procesos por hilo.

### Ejercicio 2: dotprod
-----

#### Modificaciones en el código

En primer lugar distribuimos el tamaño de los vectores de entrada usando un `MPI_Bcast()`.

```c
// Distribute vectors among processes
MPI_Bcast( &N, 1, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);
```

A continuación, cada proceso calcula sobre qué elementos le corresponde trabajar.

```c
  n_local = N / number_of_processes;
  n_resto = N % number_of_processes;
  if (rank == ROOT_PROCESS) {
    displs[0] = 0;
    for (int i = 0; i < number_of_processes - 1; i++)
    {
      sendcounts[i] = (i < n_resto) ? n_local+1 : n_local;
      displs[i+1] = displs[i] + sendcounts[i];
    }
    sendcounts[number_of_processes-1] = (number_of_processes-1 < n_resto) ? n_local+1 : n_local;
  } else {
    sendcounts[rank] = (rank < n_resto) ? n_local+1 : n_local;
  }
```

Cada proceso reserva memoria para almacenar los vectores de entrada.

```c
/* Allocate memory for vectors */
if ((x_local = (float *)malloc(sendcounts[rank] * sizeof(float))) == NULL)
    printf("Error in malloc x_local[%d]\n", sendcounts[rank]);

if ((y_local = (float *)malloc(sendcounts[rank] * sizeof(float))) == NULL)
    printf("Error in malloc y_local[%d]\n", sendcounts[rank]);
```

El proceso 0 distribuye dichos vectores.

```c
MPI_Scatterv( x, sendcounts, displs, 
                MPI_FLOAT, x_local, sendcounts[rank], MPI_FLOAT, 
                ROOT_PROCESS, MPI_COMM_WORLD);

MPI_Scatterv( y, sendcounts, displs, 
                MPI_FLOAT, y_local, sendcounts[rank], MPI_FLOAT, 
                ROOT_PROCESS, MPI_COMM_WORLD);
```

Cada proceso calcula sobre esa parte de los vectores de forma concurrente, usando un `#pragma omp parallel for`.

```c
/* Dot product operation */

dot_local = 0.;
#pragma omp parallel for reduction(+:dot_local)
for (i = 0; i < sendcounts[rank]; i++)
    dot_local += x_local[i] * y_local[i];
```

Finalmente, los procesos juntan sus resultados parciales en el proceso 0 usando `MPI_Reduce()`.

```c
MPI_Reduce( &dot_local, &dot, 
            1, MPI_FLOAT, 
            MPI_SUM, ROOT_PROCESS, MPI_COMM_WORLD);
```

#### Evaluación del rendimiento


| .....          | 1 Proc - 1 Hilo   | 1 Proc - 16 Hilos | 2 Procs - 8 Hilos | 4 Procs - 4 Hilos | 8 Procs - 2 Hilos | 16 Procs - 1 Hilo |
| :-------------:| -----------------:| -----------------:| -----------------:| -----------------:| -----------------:| -----------------:|
| dotprod        | 8.716 s  (x1)     | 5.646 s  (x1.54)  | 5.252 s  (x1.66)  | 7.963 s  (x1.09)  | 7.668 s  (x1.14)  | 7.655 s  (x1.37)  |

Podemos observar que el programa funciona mejor con más hilos y menos procesos, en concreto el programa alcanza su máximo rendimiento al usar 4 hilos por proceso o menos. El rendimiento empeora al usar 2 procesos y 8 hilos por proceso y empeora incluso más al usar un solo hilo y 16 procesos por hilo.

### Ejercicio 3: mxvnm
-----

#### Modificaciones en el código

El código se mantiene prácticamente como el original, salvo por 3 cambios:

1. Añadimos un `#pragma omp parallel for` al bucle interno del cómputo para que se ejecute de forma concurrente
2. Antes del cómputo añadimos una fase de distribución de datos, donde los datos de entrada se envían desde el proceso 0 a todos los procesos.
3. Después del cómputo añadimos una fase de recolección de resultados, donde los resultados parciales de cada proceso se envían al proceso 0, que dispone del resultado final completo al final del programa.

#### Evaluación del rendimiento

| Dimensiones        | 1 Proc - 16 Hilos | 4 Procs - 4 Hilos | 16 Procs - 1 Hilo |
|   :-------------:  | -----------------:| -----------------:| -----------------:|
| **32768 x 32768**  | 4.454 s           | 4.949 s           | 4.966 s           |
| **131072 x 8192**  | 5.646 s           | 5.037 s           | 4.088 s           |
| **8192 x 131072**  | 3.864 s           | 4.977 s           | 5.038 s           |


Para el análisis de rendimiento de este código se han elegido 3 valores para sus filas y columnas.

1. Filas  =  Columnas. La matriz tiene la misma cantidad de filas que de columnas.
2. Filas >>> Columnas. La matriz tiene muchas filas pequeñas.
3. Filas <<< Columnas. La matriz tiene pocas filas muy grandes

El programa distribuye las filas a nivel de proceso y en cada fila, procesa sus columnas de forma paralela a nivel de hilo. De esta forma los resultados experimentales se corresponden con los esperados a nivel teórico. Al aumentar la cantidad de filas respecto a las columnas una configuración con más procesos obtiene mejor rendimiento, ya que puede aprovechar mejor el paralelismo a nivel de filas. Sin embargo, al aumentar la cantidad de columnas respecto al número de filas es una versión con más hilos la que se beneficia más, ya que es la que aprovecha el paralelismo a nivel de columna. Por último, en el caso en que las filas y las columnas están niveladas, todas las configuraciones se benefician relativamente de su paralelismo.

De esta forma una configuración híbrida funciona mejor que solo OpenMP cuando hay muchas filas muy pequeñas y funciona mejor que MPI cuando hay pocas filas muy grandes.
