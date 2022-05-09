# APP_Lab1

## Introducción

algo de intro tatatatatatt...

## Vectorización

### Ejercicio 4: Jacobi
-----

#### Modificaciones en el código

En la fúnción `jacobi` existen dos dobles bucles paralelizados, uno de 'inicialización de datos' y otro más principal.

En ambos casos el bucle externo está paralelizado con un `pragma omp parallel for`, en concreto:

```c++
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

Lo principal a tener en cuenta es que en el doble bucle principal, la variable `error` es una variable de reducción. Quitando esto, la vectorización en completamente directa, añadiendo la cláusula `pragma omp simd` al buble interno. El resultado quedaría de la siguiente forma:

```c++
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


| .....        | 1 Hilo    | 2 Hilos     | 4 Hilos     | 8 Hilos     | 16 Hilos     |
| :-----------:|----------:| -----------:| -----------:| -----------:| ------------:|
| Blocking     | -.--- s   | -.--- s     | -.--- s     | -.--- s     | -.--- s      |
| Non Blocking | -.--- s   | -.--- s     | -.--- s     | -.--- s     | -.--- s      |

Observamos que el rendimiento mejora con la vectorización, en concreto se puede apreciar una mejora en el rendimiento de aproximadamente x1.75.

## Afinidad de hilos

## Programación híbrida
