#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sysexits.h>
#include "bpt.h"

/*
 *  All nodes have an array of keys, and an array of values.
 *  In non-leaf nodes, values[i] points to the node that is either
 *  greater than or equal to key[i-1] or less than key[i].
 */


void (*repair_func)(struct node *, struct node *) = NULL;

//{{{ struct node *to_node(void *n)
struct node *to_node(void *n)
{
    return (struct node*)n;
}
//}}}

//{{{int b_search(int key, int *D, int D_size)
int b_search(uint32_t key, uint32_t *D, uint32_t D_size)
{
    int lo = -1, hi = D_size, mid;
    while ( hi - lo > 1) {
        mid = (hi + lo) / 2;
        if ( D[mid] < key )
            lo = mid;
        else
            hi = mid;
    }

    return hi;
}
//}}}

//{{{ struct node *new_node()
struct node *new_node()
{
    struct node *n = (struct node *)malloc(sizeof(struct node));
    n->parent = NULL;
    n->keys = (uint32_t *) 
            malloc( (ORDER+1) * sizeof(uint32_t)); //adding one help insert
    n->num_keys = 0;
    n->is_leaf = 0;
    n->pointers = (void **) malloc((ORDER+2) * sizeof(void *));
    n->next = NULL;

    return n;
}
//}}}

//{{{ struct node *find_leaf(struct node *curr, int key)
struct node *find_leaf(struct node *curr, int key)
{
    while (curr->is_leaf != 1) {
        int i = find_insert_pos(curr, key);
        if ((i < curr->num_keys) && (curr->keys[i] == key))
            i+=1;
        curr = (struct node *)curr->pointers[i];
    }
    return curr;
}
//}}}

//{{{int find_insert_pos(struct node *leaf, int key)
int find_insert_pos(struct node *leaf, int key)
{
    return b_search(key, leaf->keys, leaf->num_keys);
}
//}}}

//{{{struct node *place_new_key_value(struct node *root,
struct node *place_new_key_value(struct node *root,
                                 struct node **target_node,
                                 int *target_key_pos,
                                 int key,
                                 void *value)
{
    int insert_key_pos = find_insert_pos(*target_node, key);

    int insert_value_pos = insert_key_pos;

    if ((*target_node)->is_leaf == 0)
        insert_value_pos += 1;

    if ((*target_node)->is_leaf == 1)
        *target_key_pos = insert_key_pos;

    // move everything over
    int i;

#if DEBUG
    fprintf(stderr, "place_new_key_value:\t"
                    "target_node:%p\t"
                    "target_node->num_keys:%d\t"
                    "insert_key_pos:%d\t"
                    "target_node->is_leaf:%d\n",
                    *target_node,
                    (*target_node)->num_keys,
                    insert_key_pos,
                    (*target_node)->is_leaf
                    );
#endif

    for (i = (*target_node)->num_keys; i > insert_key_pos; --i) {
#if DEBUG
        fprintf(stderr, "place_new_key_value:\ti:%d\tk:%d\n", 
                        i,
                        (*target_node)->keys[i-1]);
#endif
        (*target_node)->keys[i] = (*target_node)->keys[i-1];
    }

    if ((*target_node)->is_leaf == 1) {
        for (i = (*target_node)->num_keys; i > insert_value_pos; --i) 
            (*target_node)->pointers[i] = (*target_node)->pointers[i-1];
    } else {
        for (i = (*target_node)->num_keys+1; i > insert_value_pos; --i) {

#if DEBUG
            fprintf(stderr, "place_new_key_value:\ti:%d\tv:%p\n", 
                            i,
                            (*target_node)->pointers[i-1]);
#endif

            (*target_node)->pointers[i] = (*target_node)->pointers[i-1];
        }
    }


#if DEBUG
    fprintf(stderr, "place_new_key_value:\ti:%d\tkey:%d\n",
                    insert_key_pos,
                    key);
    fprintf(stderr, "place_new_key_value:\ti:%d\tvalue:%p\n",
                    insert_value_pos,
                    value);
#endif

    (*target_node)->keys[insert_key_pos] = key;
    (*target_node)->pointers[insert_value_pos] = value;

    (*target_node)->num_keys += 1;

    // If there are now too many values in the node, split it
    if ((*target_node)->num_keys > ORDER) {
        struct node *lo_result_node = NULL, *hi_result_node = NULL;
        int lo_hi_split_point = 0;
        struct node *r = split_node(root,
                                    *target_node,
                                    &lo_result_node,
                                    &hi_result_node,
                                    &lo_hi_split_point,
                                    repair_func);
        if ((*target_node)->is_leaf) {
            if (insert_key_pos < lo_hi_split_point)
                *target_node = lo_result_node;
            else {
                *target_node = hi_result_node;
                *target_key_pos = insert_key_pos - lo_hi_split_point;
            }
        }

        return r;
    } else {
        return root;
    }
}
//}}}

//{{{struct node *split_node(struct node *root, struct node *node)
struct node *split_node(struct node *root,
                        struct node *node,
                        struct node **lo_result_node,
                        struct node **hi_result_node,
                        int *lo_hi_split_point,
                        void (*repair)(struct node *, struct node *))
{

#if DEBUG
    fprintf(stderr, "split_node():\n");
#endif

    struct node *n = new_node();

#if DEBUG
    fprintf(stderr, "split_node():\tnew_node:%p\n", n);
#endif

    // set the split location
    int split_point = (ORDER + 1)/2;

    *lo_result_node = node;
    *hi_result_node = n;
    *lo_hi_split_point = split_point;

    // copy the 2nd 1/2 of the values over to the new node
    int node_i, new_node_i = 0;
    for (node_i = split_point; node_i < node->num_keys; ++node_i) {
        n->keys[new_node_i] = node->keys[node_i];
        n->pointers[new_node_i] = node->pointers[node_i];
        n->num_keys += 1;
        new_node_i += 1;
    }

    // if the node is not a leaf, the far right pointer must be coppied too
    if (node->is_leaf == 0)
        n->pointers[new_node_i] = node->pointers[node_i];

    // set status of new node
    n->is_leaf = node->is_leaf;
    n->parent = node->parent;

    node->num_keys = split_point;

    if (node->is_leaf == 0) {
#if DEBUG
        fprintf(stderr, "split_node():\tsplit non-leaf\n");
#endif
        // if the node is not a leaf, then update the parent pointer of the 
        // children
        for (node_i = 1; node_i <= n->num_keys; ++node_i) 
            ( (struct node *)n->pointers[node_i])->parent = n;
    } else {
        // if the node is a leaf, then connect the two
        n->next = node->next;
        node->next = n;

#if DEBUG
        fprintf(stderr,
                "split_node():\tsplit leaf old->next:%p new->next:%p\n",
                node->next,
                n->next);
#endif
    }

    if (repair != NULL) {
        repair(node, n);
    }

    if (node == root) {
#if DEBUG
            fprintf(stderr, "split_node():\tsplit root\tk:%d\n", n->keys[0]);
#endif

        // if we just split the root, create a new root witha single value
        struct node *new_root = new_node();
        new_root->is_leaf = 0;
        new_root->keys[0] = n->keys[0];
        new_root->pointers[0] = (void *)node; 
        new_root->pointers[1] = (void *)n; 
        new_root->num_keys += 1;
        node->parent = new_root;
        n->parent = new_root;
        return new_root;
    } else {
#if DEBUG
            fprintf(stderr, "split_node():\tsplit non-root\n");
#endif
        // if we didnt split the root, place the new value in the parent node
        return place_new_key_value(root,
                                   &(node->parent),
                                   NULL,
                                   n->keys[0],
                                   (void *)n); 
    }
}
//}}}

//{{{struct node *insert(struct node *root, int key, void *value)
struct node *insert(struct node *root,
                    int key,
                    void *value,
                    struct node **leaf,
                    int *pos)
{

#if DEBUG
    fprintf(stderr, "insert():\tk:%d\n", key);
#endif

    if (root == NULL) {
        root = new_node();
        root->is_leaf = 1;
        root->keys[0] = key;
        root->pointers[0] = value;
        root->num_keys += 1;

        *leaf = root;
        *pos = 0;

        return root;
    } else {
        *leaf = find_leaf(root, key);
#if DEBUG
        fprintf(stderr, "insert():\tleaf:%p\n", *leaf);
#endif
        root = place_new_key_value(root,
                                   leaf,
                                   pos,
                                   key,
                                   value);
        return root;
    }
}
//}}}

//{{{void print_tree(struct node *curr, int level)
void print_tree(struct node *curr, int level)
{
    int i;
    for (i = 0; i < level; ++i)
        printf(" ");

    printf("keys %p(%d)(%d):", curr,curr->num_keys, curr->is_leaf);

    for (i = 0; i < curr->num_keys; ++i)
        printf(" %d", curr->keys[i]);
    printf("\n");

    for (i = 0; i < level; ++i)
        printf(" ");
    printf("values:");

    if (curr->is_leaf == 0) {
        for (i = 0; i <= curr->num_keys; ++i)
            printf(" %p", (struct node *)curr->pointers[i]);
    } else {
        for (i = 0; i < curr->num_keys; ++i) {
            int *v = (int *)curr->pointers[i];
            printf(" %d", *v);
        }
    }
    printf("\n");

    if (curr->is_leaf == 0) {
        for (i = 0; i <= curr->num_keys; ++i)
            print_tree((struct node *)curr->pointers[i], level+1);
    }
}
//}}}


void print_node(struct node *n)
{
    int i;
    if (n->is_leaf)
        printf("+");
    else
        printf("-");
    printf("\t%p\t%p\t",n, n->parent);

    for (i = 0; i < n->num_keys; ++i) {
        if (i!=0)
            printf(" ");
        printf("%u", n->keys[i]);
    }
    printf("\n");
}

//{{{void print_values(struct node *root)
void print_values(struct node *root)
{
    struct node *curr = root;
    while (curr->is_leaf != 1) {
        curr = (struct node *)curr->pointers[0];
    }

    printf("values: ");
    int i, *v;
    while (1) {
        for (i = 0; i < curr->num_keys; ++i) {
            v = (int *)curr->pointers[i];
            printf(" %d", *v);
        }
        if (curr->next == NULL)
            break;
        else
            curr = curr->next;
    }
    printf("\n");
}
//}}}

//{{{ void *find(struct node *root, int key) 
void *bpt_find(struct node *root, struct node **leaf, int key) 
{
    *leaf = find_leaf(root, key);
    int insert_key_pos = find_insert_pos(*leaf, key);
#if DEBUG
    fprintf(stderr,
            "key:%d pos:%d num_key:%d\n",
            key,
            insert_key_pos,
            (*leaf)->num_keys);
#endif

    if ((insert_key_pos + 1) > (*leaf)->num_keys)
        return NULL;
    else if (key != (*leaf)->keys[insert_key_pos])
        return NULL;
    else
        return (*leaf)->pointers[insert_key_pos];
}
//}}}

//{{{ void destroy_tree(struct node **curr)
void destroy_tree(struct node **curr)
{
#if 0
    if (*curr == NULL) {
        return;
    } else if ((*curr)->is_leaf == 1) {
        fprintf(stderr, "l\t%p\n", *curr);
        free((*curr)->keys);
        free((*curr)->pointers);
        free(*curr);
        *curr = NULL;
        return;
    } else {
        fprintf(stderr, "n\t%p\n", *curr);
        uint32_t i;
        for (i = 0; i <= (*curr)->num_keys; ++i) 
            destroy_tree((struct node **)&((*curr)->pointers[i]));
        //free(*curr);
        *curr = NULL;
        return;
    } 
#endif
}
//}}}

#if 0
//{{{void store(struct node *root, char *file_name)
void store(struct node *root, char *file_name)
{
    FILE *f = fopne(file_name, "wb");

    if (!f)
        err(EX_IOERR, "Could not write to '%s'.", file_name);
}
//}}}
#endif
