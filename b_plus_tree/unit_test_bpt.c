#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "unity.h"
#include <math.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "bpt.h"

//{{{ int uint32_t_cmp(const void *a, const void *b)
static int uint32_t_cmp(const void *a, const void *b)
{
    uint32_t _a = *((uint32_t *)a), _b = *((uint32_t *)b);

    if (_a < _b)
        return -1;
    else if (_a > _b)
        return 1;
    else

    return 0;
}
//}}}

void setUp(void)
{
}

void tearDown(void)
{
}

//{{{ void test_b_search(void)
void test_b_search(void)
{
    uint32_t D[10] = {2, 3, 4, 6, 8, 10, 12, 14, 16, 18};
    uint32_t A[20] = {0, // 0
                      0, // 1
                      0, // 2
                      1, // 3
                      2, // 4
                      3, // 5
                      3, // 6
                      4, // 7
                      4, // 8
                      5, // 9
                      5, // 10
                      6, // 11
                      6, // 12
                      7, // 13
                      7, // 14
                      8, // 15
                      8, // 16
                      9, // 17
                      9, // 18
                      10}; // 19

    int i;
    for (i = 0; i < 20; ++i)
        TEST_ASSERT_EQUAL(A[i], b_search(i, D, 10));
}
//}}}

//{{{ void tet_new_node(void)
void test_new_node(void)
{
    struct node *n = new_node();
    TEST_ASSERT_EQUAL(NULL, n->parent);
    TEST_ASSERT_EQUAL(0, n->num_keys);
    TEST_ASSERT_EQUAL(0, n->is_leaf);
    TEST_ASSERT_EQUAL(NULL, n->next);
    free(n->keys);
    free(n->pointers);
    free(n);
}
//}}}

//{{{ void test_find_leaf(void)
void test_find_leaf(void)
{

    struct node *l1 = new_node();
    l1->is_leaf = 1;
    l1->num_keys = 4;
    l1->keys[0] = 1;
    l1->keys[1] = 2;
    l1->keys[2] = 3;
    l1->keys[3] = 4;

    struct node *l2 = new_node();
    l2->is_leaf = 1;
    l2->num_keys = 4;
    l2->keys[0] = 5;
    l2->keys[1] = 6;
    l2->keys[2] = 7;
    l2->keys[3] = 8;

    struct node *l3 = new_node();
    l3->is_leaf = 1;
    l3->num_keys = 4;
    l3->keys[0] = 9;
    l3->keys[1] = 10;
    l3->keys[2] = 11;
    l3->keys[3] = 12;

    l1->next = l2;
    l2->next = l3;

    struct node *n1 = new_node();
    n1->keys[0] = 5;
    n1->num_keys = 1;
    n1->pointers[0] = (void *)l1;
    n1->pointers[1] = (void *)l2;
    l1->parent = n1;
    l2->parent = n1;

    struct node *root = new_node();
    root->keys[0] = 9;
    root->num_keys = 1;
    root->pointers[0] = (void *)n1;
    root->pointers[1] = (void *)l3;
    n1->parent = root;
    l3->parent = root;

    /*
    fprintf(stderr, "l1:%p\t"
                    "l2:%p\t"
                    "l3:%p\t"
                    "n1:%p\t"
                    "root:%p\n",
                    l1, l2, l3, n1, root);
    */

    int i;
    for (i = 1; i <= 4; ++i)
        TEST_ASSERT_EQUAL(l1, find_leaf(root, i));

    for (i = 5; i <= 8; ++i) 
        TEST_ASSERT_EQUAL(l2, find_leaf(root, i));

    for (i = 9; i <= 12; ++i) 
        TEST_ASSERT_EQUAL(l3, find_leaf(root, i));

    destroy_tree(&root);
}
//}}}

//{{{void test_split_node_non_root_parent_has_room(void)
void test_split_node_non_root_parent_has_room(void)
{
    int V[5] = {2,3,4,5,6};
    struct node *l1 = new_node();
    l1->is_leaf = 1;
    l1->num_keys = 4;
    l1->keys[0] = 1;
    l1->keys[1] = 2;
    l1->keys[2] = 3;
    l1->keys[3] = 4;

    l1->pointers[0] = (void*)V;
    l1->pointers[1] = (void*)(V + 1);
    l1->pointers[2] = (void*)(V + 2);
    l1->pointers[3] = (void*)(V + 2);

    struct node *n1 = new_node();
    n1->keys[0] = 6;
    n1->num_keys = 1;
    n1->pointers[0] = (void *)l1;
    l1->parent = n1;

    struct node *root = new_node();
    root->keys[0] = 9;
    root->num_keys = 1;
    root->pointers[0] = (void *)n1;
    n1->parent = root;

    l1->num_keys = 5;
    l1->keys[4] = 5;


    struct node *lo, *hi;
    int split;
    root = split_node(root, l1, &lo, &hi, &split, NULL);

    TEST_ASSERT_EQUAL(l1, lo);
    TEST_ASSERT_EQUAL(l1->next, hi);
    TEST_ASSERT_EQUAL(split, 2);

    TEST_ASSERT_EQUAL(2, l1->num_keys);
    TEST_ASSERT_EQUAL(3, l1->next->num_keys);
    TEST_ASSERT_EQUAL(2, n1->num_keys);

    int A_n1[2] = {3,6};

    int i;
    for (i = 0; i < n1->num_keys; ++i)
        TEST_ASSERT_EQUAL(A_n1[i], n1->keys[i]);

    TEST_ASSERT_EQUAL(l1, find_leaf(root, 1));
    TEST_ASSERT_EQUAL(l1, find_leaf(root, 2));
    TEST_ASSERT_EQUAL(l1->next, find_leaf(root, 3));
    TEST_ASSERT_EQUAL(l1->next, find_leaf(root, 4));
    TEST_ASSERT_EQUAL(l1->next, find_leaf(root, 5));

    TEST_ASSERT_EQUAL(l1, n1->pointers[0]);
    TEST_ASSERT_EQUAL(l1->next, n1->pointers[1]);

    free(l1->keys);
    free(l1->pointers);
    free(l1);
    free(n1->keys);
    free(n1->pointers);
    free(n1);
    free(root->keys);
    free(root->pointers);
    free(root);
    free(hi->keys);
    free(hi->pointers);
    free(hi);
}
//}}}

//{{{void test_split_node_root(void)
void test_split_node_root(void)
{
    int V[5] = {2,3,4,5,6};
    struct node *root = new_node();
    root->is_leaf = 1;
    root->num_keys = 5;
    root->keys[0] = 1;
    root->keys[1] = 2;
    root->keys[2] = 3;
    root->keys[3] = 4;
    root->keys[4] = 5;

    root->pointers[0] = (void*)V;
    root->pointers[1] = (void*)(V + 1);
    root->pointers[2] = (void*)(V + 2);
    root->pointers[3] = (void*)(V + 2);
    root->pointers[4] = (void*)(V + 3);

    struct node *p_root = root;

    struct node *lo, *hi;
    int split;
    struct node *new_root = split_node(root, p_root, &lo, &hi, &split, NULL);

    TEST_ASSERT_EQUAL(p_root, lo);
    TEST_ASSERT_EQUAL(p_root->next, hi);
    TEST_ASSERT_EQUAL(split, 2);


    TEST_ASSERT_EQUAL(2, p_root->num_keys);
    TEST_ASSERT_EQUAL(3, p_root->next->num_keys);

    destroy_tree(&new_root);
}
//}}}

//{{{void test_insert(void)
void test_insert(void)
{
    /*
     * 2,3,4,5
     *
     * 4
     *  2,3 4,5,6
     *
     * 4,6
     *  2,3 4,5 6,7,8
     *
     * 4,6,8
     *  2,3 4,5 6,7 8,9,10
     *
     * 4,6,8,10
     *  2,3 4,5 6,7 8,9 10,11,12
     *
     * 8 
     *  4,6 8,10,12
     *   2,3 4,5 6,7 8,9 10,11 12,13,14
     */
    int V[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int v=0;

    struct node *root = NULL;
    struct node *leaf = NULL;
    int pos;


    root = insert(root, 2, (void *)(V + v++), &leaf, &pos);

    TEST_ASSERT_EQUAL(1, root->num_keys);
    TEST_ASSERT_EQUAL(2, root->keys[0]);
    TEST_ASSERT_EQUAL(1, root->is_leaf);
    TEST_ASSERT_EQUAL(root, leaf);
    TEST_ASSERT_EQUAL(0, pos);

    // 4
    //  2,3 4,5,6
    root = insert(root, 3, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(root, leaf);
    TEST_ASSERT_EQUAL(1, pos);

    root = insert(root, 4, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(root, leaf);
    TEST_ASSERT_EQUAL(2, pos);
    root = insert(root, 5, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(root, leaf);
    TEST_ASSERT_EQUAL(3, pos);
    root = insert(root, 6, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(2, pos);

    TEST_ASSERT_FALSE(root == leaf);
    TEST_ASSERT_EQUAL(root->pointers[1], leaf);

    TEST_ASSERT_EQUAL(1, root->num_keys);
    TEST_ASSERT_EQUAL(4, root->keys[0]);
    TEST_ASSERT_EQUAL(0, root->is_leaf);

    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[0])->num_keys);
    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[0])->keys[0]);
    TEST_ASSERT_EQUAL(3, ((struct node*)root->pointers[0])->keys[1]);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[0])->is_leaf);
    TEST_ASSERT_EQUAL(root->pointers[1], 
                      ((struct node*)root->pointers[0])->next);

    TEST_ASSERT_EQUAL(3, ((struct node*)root->pointers[1])->num_keys);
    TEST_ASSERT_EQUAL(4, ((struct node*)root->pointers[1])->keys[0]);
    TEST_ASSERT_EQUAL(5, ((struct node*)root->pointers[1])->keys[1]);
    TEST_ASSERT_EQUAL(6, ((struct node*)root->pointers[1])->keys[2]);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[1])->is_leaf);
    TEST_ASSERT_EQUAL(NULL, ((struct node*)root->pointers[1])->next);

    // 4,6
    //  2,3 4,5 6,7,8
    root = insert(root, 7, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(root->pointers[1], leaf);
    TEST_ASSERT_EQUAL(3, pos);
    root = insert(root, 8, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(root->pointers[2], leaf);
    TEST_ASSERT_EQUAL(2, pos);
 
    TEST_ASSERT_EQUAL(2, root->num_keys);
    TEST_ASSERT_EQUAL(4, root->keys[0]);
    TEST_ASSERT_EQUAL(6, root->keys[1]);

    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[0])->num_keys);
    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[0])->keys[0]);
    TEST_ASSERT_EQUAL(3, ((struct node*)root->pointers[0])->keys[1]);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[0])->is_leaf);
    TEST_ASSERT_EQUAL(root->pointers[1], 
                      ((struct node*)root->pointers[0])->next);

    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[1])->num_keys);
    TEST_ASSERT_EQUAL(4, ((struct node*)root->pointers[1])->keys[0]);
    TEST_ASSERT_EQUAL(5, ((struct node*)root->pointers[1])->keys[1]);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[1])->is_leaf);
    TEST_ASSERT_EQUAL(root->pointers[2], 
                      ((struct node*)root->pointers[1])->next);

    TEST_ASSERT_EQUAL(3, ((struct node*)root->pointers[2])->num_keys);
    TEST_ASSERT_EQUAL(6, ((struct node*)root->pointers[2])->keys[0]);
    TEST_ASSERT_EQUAL(7, ((struct node*)root->pointers[2])->keys[1]);
    TEST_ASSERT_EQUAL(8, ((struct node*)root->pointers[2])->keys[2]);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[2])->is_leaf);
    TEST_ASSERT_EQUAL(NULL, ((struct node*)root->pointers[2])->next);

    // 8
    //  4,6 8,10,12
    //   2,3 4,5 6,7 8,9 10,11 12,13,14

    root = insert(root, 9, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 10, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 11, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 12, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 13, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 14, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(((struct node*)root->pointers[1])->pointers[3], leaf);
 
    TEST_ASSERT_EQUAL(1, root->num_keys);
    TEST_ASSERT_EQUAL(8, root->keys[0]);

    //  4,6 8,10,12
    struct node *n1 = (struct node *)root->pointers[0];
    struct node *n2 = (struct node *)root->pointers[1];

    TEST_ASSERT_EQUAL(2, n1->num_keys);
    TEST_ASSERT_EQUAL(4, n1->keys[0]);
    TEST_ASSERT_EQUAL(6, n1->keys[1]);
    TEST_ASSERT_EQUAL(0, n1->is_leaf);
    TEST_ASSERT_EQUAL(NULL, n1->next);

    TEST_ASSERT_EQUAL(3, n2->num_keys);
    TEST_ASSERT_EQUAL(8, n2->keys[0]);
    TEST_ASSERT_EQUAL(10, n2->keys[1]);
    TEST_ASSERT_EQUAL(12, n2->keys[2]);
    TEST_ASSERT_EQUAL(0, n2->is_leaf);
    TEST_ASSERT_EQUAL(NULL, n2->next);

    //   2,3 4,5 6,7 8,9 10,11 12,13,14
    struct node *l1 = (struct node *)n1->pointers[0];
    struct node *l2 = (struct node *)n1->pointers[1];
    struct node *l3 = (struct node *)n1->pointers[2];
    //struct node *l3 = (struct node *)n2->pointers[0];
    struct node *l4 = (struct node *)n2->pointers[1];
    struct node *l5 = (struct node *)n2->pointers[2];
    struct node *l6 = (struct node *)n2->pointers[3];

    TEST_ASSERT_EQUAL(2, l1->num_keys);
    TEST_ASSERT_EQUAL(2, l1->keys[0]);
    TEST_ASSERT_EQUAL(3, l1->keys[1]);
    TEST_ASSERT_EQUAL(1, l1->is_leaf);
    TEST_ASSERT_EQUAL(l2, l1->next);

    TEST_ASSERT_EQUAL(2, l2->num_keys);
    TEST_ASSERT_EQUAL(4, l2->keys[0]);
    TEST_ASSERT_EQUAL(5, l2->keys[1]);
    TEST_ASSERT_EQUAL(1, l2->is_leaf);
    TEST_ASSERT_EQUAL(l3, l2->next);

    TEST_ASSERT_EQUAL(2, l3->num_keys);
    TEST_ASSERT_EQUAL(6, l3->keys[0]);
    TEST_ASSERT_EQUAL(7, l3->keys[1]);
    TEST_ASSERT_EQUAL(1, l3->is_leaf);
    TEST_ASSERT_EQUAL(l4, l3->next);

    TEST_ASSERT_EQUAL(2, l4->num_keys);
    TEST_ASSERT_EQUAL(8, l4->keys[0]);
    TEST_ASSERT_EQUAL(9, l4->keys[1]);
    TEST_ASSERT_EQUAL(1, l4->is_leaf);
    TEST_ASSERT_EQUAL(l5, l4->next);

    TEST_ASSERT_EQUAL(2, l5->num_keys);
    TEST_ASSERT_EQUAL(10, l5->keys[0]);
    TEST_ASSERT_EQUAL(11, l5->keys[1]);
    TEST_ASSERT_EQUAL(1, l5->is_leaf);
    TEST_ASSERT_EQUAL(l6, l5->next);

    TEST_ASSERT_EQUAL(3, l6->num_keys);
    TEST_ASSERT_EQUAL(12, l6->keys[0]);
    TEST_ASSERT_EQUAL(13, l6->keys[1]);
    TEST_ASSERT_EQUAL(14, l6->keys[2]);
    TEST_ASSERT_EQUAL(1, l6->is_leaf);
    TEST_ASSERT_EQUAL(NULL, l6->next);

    destroy_tree(&root);
}
//}}}

//{{{void test_insert(void)
void test_insert_out_of_order(void)
{
    /*
     * 2,3,4,5
     *
     * 4
     *  2,3 4,5,6
     *
     * 4,6
     *  2,3 4,5 6,7,8
     *
     * 4,6,8
     *  2,3 4,5 6,7 8,9,10
     *
     * 4,6,8,10
     *  2,3 4,5 6,7 8,9 10,11,12
     *
     * 8 
     *  4,6 8,10,12
     *   2,3 4,5 6,7 8,9 10,11 12,13,14
     */
    int V[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int v=0;

    struct node *root = NULL;
    struct node *leaf = NULL;
    int pos;


    /*
     * 4
     *  2,3 4,5,6
     */
    root = insert(root, 3, (void *)(V + 0), &leaf, &pos);
    root = insert(root, 5, (void *)(V + 1), &leaf, &pos);
    root = insert(root, 4, (void *)(V + 2), &leaf, &pos);
    root = insert(root, 6, (void *)(V + 3), &leaf, &pos);
    root = insert(root, 2, (void *)(V + 4), &leaf, &pos);

    TEST_ASSERT_EQUAL(1, root->num_keys);

    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[0])->num_keys);
    TEST_ASSERT_EQUAL(3, ((struct node*)root->pointers[1])->num_keys);

    struct node *n = (struct node*)root->pointers[0];
    TEST_ASSERT_EQUAL(V[4], *((uint32_t *)(n->pointers[0])));
    TEST_ASSERT_EQUAL(V[0], *((uint32_t *)(n->pointers[1])));

    n = (struct node*)root->pointers[1];
    TEST_ASSERT_EQUAL(V[2], *((uint32_t *)(n->pointers[0])));
    TEST_ASSERT_EQUAL(V[1], *((uint32_t *)(n->pointers[1])));
    TEST_ASSERT_EQUAL(V[3], *((uint32_t *)(n->pointers[2])));

    destroy_tree(&root);
}
//}}}

//{{{void test_insert_id_updated_node(void)
void test_insert_id_updated_node(void)
{
    int V[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int v=0;

    struct node *root = NULL;
    struct node *leaf;
    int pos;

    root = insert(root, 2, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 5, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 10, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 15, (void *)(V + v++), &leaf, &pos);

    root = insert(root, 1, (void *)(V + v++), &leaf, &pos);
    TEST_ASSERT_EQUAL(root->pointers[0], leaf);

    destroy_tree(&root);

}
//}}}

//{{{void test_find(void)
void test_find(void)
{
    /*
     *  -> 2
     *  -> 4
     *  -> 6
     *  -> 8
     *  2,4,6,8
     *
     *  -> 10 
     *  6
     *  2,4, 6,8,10
     *
     *  -> 12 
     *  6
     *  2,4, 6,8,10,12
     *
     *  -> 14 
     *  6,10
     *  2,4, 6,8  10,12,14
     *
     *  -> 16 
     *  6,10
     *  2,4, 6,8  10,12,14,16
     *
     *  -> 18 
     *  6,10,14
     *  2,4, 6,8  10,12 14,16,18
     *
     *  -> 20 
     *  6,10,14
     *  2,4, 6,8  10,12 14,16,18,20
     *
     *  -> 22 
     *  6,10,14,18
     *  2,4, 6,8  10,12 14,16 18,20,22
     *
     *  -> 24 
     *  6,10,14,18
     *  2,4, 6,8  10,12 14,16 18,20,22,24
     *
     *  -> 26 
     *  14
     *  6,10 14,18,22
     *  2,4, 6,8  10,12 14,16 18,20 22,24,26
     *
     */

    int V[13] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
    int v=0;

    struct node *root = NULL;
    struct node *leaf;

    int pos;

    int i;
    for (i = 0; i < 13; ++i)
        root = insert(root, (i+1)*2, (void *)(V + v++), &leaf, &pos);

    int *r;
    v=0;
    for (i = 1; i <= 13; ++i) {
        r = (int *)bpt_find(root, &leaf, i);
        if (i % 2 != 0)
            TEST_ASSERT_EQUAL(NULL, r);
        else 
            TEST_ASSERT_EQUAL(i/2, *r);
    }

    destroy_tree(&root);
}
//}}}

//{{{void test_split_repair(void)
void decrement_repair(struct node *a, struct node *b)
{
    /*
    struct node 
    {
        struct node *parent;
        uint32_t *keys, num_keys, is_leaf;
        void **pointers;
        struct node *next;
    };
    */

    uint32_t i;
    for (i = 0; i < a->num_keys; ++i)
        a->keys[i] = a->keys[i] - 1;

    for (i = 0; i < b->num_keys; ++i)
        b->keys[i] = b->keys[i] + 1;

}

void test_split_repair(void)
{
    /*
     * 2,3,4,5
     *
     * 4
     *  2,3 4,5,6
     *
     * 4,6
     *  2,3 4,5 6,7,8
     *
     * 4,6,8
     *  2,3 4,5 6,7 8,9,10
     *
     * 4,6,8,10
     *  2,3 4,5 6,7 8,9 10,11,12
     *
     * 8 
     *  4,6 8,10,12
     *   2,3 4,5 6,7 8,9 10,11 12,13,14
     */
    int V[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int v=0;

    struct node *root = NULL;
    struct node *leaf = NULL;

    int pos;

    repair_func = decrement_repair;

    root = insert(root, 2, (void *)(V + v++), &leaf, &pos);

    TEST_ASSERT_EQUAL(1, root->num_keys);
    TEST_ASSERT_EQUAL(2, root->keys[0]);
    TEST_ASSERT_EQUAL(1, root->is_leaf);

    // 4
    //  2,3 4,5,6
    root = insert(root, 3, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 4, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 5, (void *)(V + v++), &leaf, &pos);

    TEST_ASSERT_EQUAL(4, root->num_keys);
    TEST_ASSERT_EQUAL(2, root->keys[0]);
    TEST_ASSERT_EQUAL(1, root->is_leaf);

    root = insert(root, 6, (void *)(V + v++), &leaf, &pos);

    // Using a silly split repair function just to test, 
    // -1 the keys in the left node and + those in the right

    // 5
    //  1,2 5,6,7
    TEST_ASSERT_EQUAL(1, root->num_keys);
    TEST_ASSERT_EQUAL(5, root->keys[0]);
    TEST_ASSERT_EQUAL(0, root->is_leaf);

    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[0])->num_keys);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[0])->keys[0]);
    TEST_ASSERT_EQUAL(2, ((struct node*)root->pointers[0])->keys[1]);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[0])->is_leaf);
    TEST_ASSERT_EQUAL(root->pointers[1], 
                      ((struct node*)root->pointers[0])->next);

    TEST_ASSERT_EQUAL(3, ((struct node*)root->pointers[1])->num_keys);
    TEST_ASSERT_EQUAL(5, ((struct node*)root->pointers[1])->keys[0]);
    TEST_ASSERT_EQUAL(6, ((struct node*)root->pointers[1])->keys[1]);
    TEST_ASSERT_EQUAL(7, ((struct node*)root->pointers[1])->keys[2]);
    TEST_ASSERT_EQUAL(1, ((struct node*)root->pointers[1])->is_leaf);
    TEST_ASSERT_EQUAL(NULL, ((struct node*)root->pointers[1])->next);

    repair_func = NULL;

    destroy_tree(&root);
}
//}}}

//{{{ void test_destroy_tree(void)
void test_destroy_tree(void)
{
    /*
     * 2,3,4,5
     *
     * 4
     *  2,3 4,5,6
     *
     * 4,6
     *  2,3 4,5 6,7,8
     *
     * 4,6,8
     *  2,3 4,5 6,7 8,9,10
     *
     * 4,6,8,10
     *  2,3 4,5 6,7 8,9 10,11,12
     *
     * 8 
     *  4,6 8,10,12
     *   2,3 4,5 6,7 8,9 10,11 12,13,14
     */
    int V[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int v=0;

    struct node *root = NULL;
    struct node *leaf = NULL;
    int pos;

    root = insert(root, 2, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 3, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 4, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 5, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 6, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 7, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 8, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 9, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 10, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 11, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 12, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 13, (void *)(V + v++), &leaf, &pos);
    root = insert(root, 14, (void *)(V + v++), &leaf, &pos);

    destroy_tree(&root);
}
//}}}

//{{{ void test_rand_test(void)
void test_bpt_find(void)
{
    repair_func = NULL;
    struct node *root = NULL;
    struct node *leaf = NULL;
    int pos;
    uint32_t size = 1000;

    uint32_t *d = (uint32_t *)malloc(size * sizeof(uint32_t));
    uint32_t *v = (uint32_t *)malloc(size * sizeof(uint32_t));

    time_t t = time(NULL);
    srand(t);

    uint32_t i, j;
    for (i = 0; i < size; ++i) {
        d[i] = rand() % 100000;
        v[i] = d[i] /2;
        root = insert(root, d[i], (void *)(v + i), &leaf, &pos);
    }

    uint32_t *r;
    for (i = 0; i < size; ++i) {
        r = (uint32_t *)bpt_find(root, &leaf, d[i]);
        TEST_ASSERT_EQUAL(d[i]/2, *r);
    }

    free(d);
    free(v);
    destroy_tree(&root);
}
//}}}
