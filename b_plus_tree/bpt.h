#ifndef __BPT_H__
#define __BPT_H__

#define ORDER 4

struct node 
{
    struct node *parent;
    int *keys, num_keys, is_leaf;
    void **pointers;
    struct node *next;
};

struct node *to_node(void *n);

struct node *insert(struct node *root, int key, void *value);

struct node *place_new_key_value(struct node *root,
                                 struct node *target_node,
                                 int key,
                                 void *value);

struct node *new_node();

void print_tree(struct node *curr, int level);

int search(int key, int *D, int D_size);

struct node *find_leaf(struct node *curr, int key);

int find_insert_pos(struct node *leaf, int key);

struct node *split_node(struct node *root, struct node *node);

#endif
