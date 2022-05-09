TARGETS=ex1 ex2 ex3
MATH_TARGETS=jacobi jacobiVect

CC=gcc

all : $(TARGETS) $(MATH_TARGETS)

$(TARGETS): % : %.c
	$(CC) -O2 -o $@ -fopenmp -fopt-info-vec -march=native $^  

$(MATH_TARGETS): % : %.c
	$(CC) -O2 -o $@ -fopenmp -fopt-info-vec -march=native $^ -lm

clean:
	rm -f $(TARGETS) $(MATH_TARGETS) *.o