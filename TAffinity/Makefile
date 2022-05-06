TARGETS=multf dotprod

CC=gcc

all : $(TARGETS)

$(TARGETS): % : %.c
	$(CC) -O2 -o $@ -fopenmp -fopt-info-vec -march=native $^  

clean:
	rm -f $(TARGETS) *.o
