#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "timer.h"

// It returns a random permutation of 0..n-1
uint32_t *rpermute(uint32_t n) {
    uint32_t *A = malloc(n*sizeof(uint32_t));
    int i;
    for (i = 0; i < n; i++)
        A[i] = i;
    for (i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        uint32_t temp = A[j];
        A[j] = A[i];
        A[i] = temp;
    }
    return A;
}

int usage(char *prog)
{
    fprintf(stderr, "usage:\t%s <options>\n"
                    "\t\t-n\tlist size\n"
                    "\t\t-d\tbit density [0,100]\n",
                    prog);
    return 1;
}

void set_bit(uint32_t *A, uint32_t pos)
{
    A[pos/32] |= 1 << (31 - (pos %32));
}

uint32_t get_bit(uint32_t *A, uint32_t pos)
{
    return (A[pos/32] >> (31 - (pos % 32))) & 1;
}


//{{{ void basic(uint32_t *A, uint32_t *B, uint32_t *order, uint32_t N,
void basic(uint32_t *A, uint32_t *B, uint32_t *order, uint32_t N, uint32_t T)
{
    uint32_t i,j,k;
    for (k = 0; k < T; ++k) {
        start();
        for (i = 0; i < N; ++i) {
            for (j = 0; j <= 31; ++j) {
                if (get_bit(A,32*i+j))
                    set_bit(B,order[32*i+j]);
            }
        }
        stop();
        printf("%lu\n", report());
    }
}
//}}}

//{{{ void basic_clz(uint32_t *A,
void basic_clz(uint32_t *A,
               uint32_t *B,
               uint32_t *order,
               uint32_t N,
               uint32_t T)
{
    uint32_t i,j,k;
    for (k = 0; k < T; ++k) {
        start();
        uint32_t word, bit_count, clz;
        for (i = 0; i < N; ++i) {
            word = A[i];
            bit_count = __builtin_popcount(word);
            for (j = 0; j < bit_count; ++j) {
                clz =  __builtin_clz(word);
                set_bit(B,order[32*i+clz]);
                word &= ~(1 << (31 - clz));
            }
        }
        stop();
        printf("%lu\n", report());
    }
}
//}}}

//{{{ void basic_openmp(uint32_t *A, uint32_t *B, uint32_t *order, uint32_t N,
void basic_openmp(uint32_t *A,
                  uint32_t *B,
                  uint32_t *order,
                  uint32_t N,
                  uint32_t T)
{
    uint32_t i,k;
    for (k = 0; k < T; ++k) {
        start();

        #pragma omp parallel for
        for (i = 0; i < N; ++i) {
            uint32_t j;
            for (j = 0; j <= 31; ++j) {
                if (get_bit(A,32*i+j))
                    set_bit(B,order[32*i+j]);
            }
        }
        stop();
        printf("%lu\n", report());
    }
}
//}}}

#if 0
struct basic_pthread_args
{
    uint32_t *A, *B, *order, A_i;
};

void basic_pthread(uint32_t *A,
                   uint32_t *B,
                   uint32_t *order,
                   uint32_t N,
                   uint32_t T,
                   uint32_t P)
{
    uint32_t i,j,k;
    for (k = 0; k < T; ++k) {
        start();

        pthread_t treads[P];

        for (i = 0; i < N; ++i) {
            
            for (j = 0; j <= 31; ++j) {
                if (get_bit(A,32*i+j))
                    set_bit(B,order[32*i+j]);
            }

        }
        stop();
        printf("%lu\n", report());
    }
}

void basic_pthread_work(void *args)
{
    struct basic_pthread_args *p = (struct basic_pthread_args *)args;
 
    uint32_t j;
    for (j = 0; j <= 31; ++j) {
        if (get_bit(p->A,32*(p->A_i)+j))
            set_bit(p->B,p->order[32*(p->A_i)+j]);
    }
}
#endif

int main(int argc, char **argv)
{
    //{{{ setup
    int n_is_set = 0,
        d_is_set = 0,
        s_is_set = 0,
        h_is_set = 0;

    uint32_t N, seed, d;

    int c;

    while ((c = getopt (argc, argv, "hn:d:s:")) != -1) {
        switch (c) {
        case 's':
            s_is_set = 1;
            seed = atoi(optarg);
            break;
        case 'h':
            h_is_set = 1;
            break;
        case 'n':
            n_is_set = 1;
            N = atoi(optarg);
            break;
        case 'd':
            d_is_set = 1;
            d = atoi(optarg);
            break;
        case '?':
            if ( (optopt == 'n') ||
                 (optopt == 's') ||
                 (optopt == 'd') )
                fprintf (stderr, "Option -%c requires an argument.\n",
                         optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        default:
            return usage(argv[0]);
        }
    }

    if (h_is_set)
        return usage(argv[0]);

    if (!d_is_set) {
        fprintf(stderr, "Desnity is not set.\n");
        return usage(argv[0]);
    }

    if (!n_is_set) {
        fprintf(stderr, "List size is not set.\n");
        return usage(argv[0]);
    }

    if (s_is_set)
        srand(seed);
    else
        srand(time(NULL));


    uint32_t *A = (uint32_t *) calloc(N, sizeof(uint32_t));

    uint32_t *order = rpermute(N * 32);

    uint32_t i,j,T;
    for (i = 0; i < N; ++i) {
        for (j = 0; j <= 31; ++j) {
            uint32_t r = rand() % 100;
            if ( r <= d)
                A[i] |= 1 << (31 - j);
        }
    }
    //}}}


    uint32_t *B = (uint32_t *) calloc(N, sizeof(uint32_t));
    basic(A, B, order, N, 3);
    printf("\n");

    uint32_t *C = (uint32_t *) calloc(N, sizeof(uint32_t));
    basic_clz(A, C, order, N, 3);
    printf("\n");



    uint32_t p_A = 0, p_B =0, p_C =0;
    for (i = 0; i < N; ++i) {
        p_A += __builtin_popcount(A[i]);
        p_B += __builtin_popcount(B[i]);
        p_C += __builtin_popcount(C[i]);
    }

    printf("%u\t%u\t%u\n\n", p_A, p_B, p_C);

    for (i = 0; i < N; ++i) {
        if (B[i] != C[i]) {
            printf("%u\t%u\t%u\n", i, B[i], C[i]);
        }
    }

    return 0;
}
