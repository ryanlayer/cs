UNITY_ROOT=../unity
#DFLAGS=-DDEBUG 

all: bpt_test unit_test_bpt

bpt_test: bpt_test.c bpt.c
	gcc -g -o bpt_test bpt_test.c bpt.c $(DFLAGS)

unit_test_bpt: unit_test_bpt.c bpt.c
	ruby $(UNITY_ROOT)/generate_test_runner.rb unit_test_bpt.c
	gcc -I$(UNITY_ROOT) -Wuninitialized \
	    $(DFLAGS) -DTEST $(UNITY_ROOT)/unity.c \
	    -Werror -g -o unit_test_bpt \
	    unit_test_bpt.c unit_test_bpt_Runner.c bpt.c 
	rm unit_test_bpt_Runner.c
	./unit_test_bpt

clean:
	rm -f bpt_test unit_test_bpt
