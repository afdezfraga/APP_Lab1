TARGETS=dotprod_OMPI mxvnm_OMPI
MATH_TARGETS=Eval_pi_integral_OMPI
MPICC=mpicc

all: $(TARGETS) $(MATH_TARGETS)

$(TARGETS): % : %.c
	$(MPICC) -fopenmp -o $@ $^

$(MATH_TARGETS): % : %.c
	$(MPICC) -fopenmp -o $@ $^ -lm

clean:
	rm -f $(TARGETS) $(MATH_TARGETS) *.o
