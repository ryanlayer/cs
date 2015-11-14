#ifndef __BPT_H__
#define __BPT_H__

#define ORDER 4


struct node 
{
    struct node *parent;
    uint32_t *keys, num_keys, is_leaf;
    void **pointers;
    struct node *next;
};

void (*repair_func)(struct node *, struct node *);

void (*append_func)(void *, void **);

struct node *to_node(void *n);

struct node *insert(struct node *root,
                    int key,
                    void *value,
                    struct node **leaf,
                    int *pos);

struct node *place_new_key_value(struct node *root,
                                 struct node **target_node,
                                 int *target_key_pos,
                                 int key,
                                 void *value);

struct node *new_node();

void print_tree(struct node *curr, int level);

void print_node(struct node *node);

int b_search(uint32_t key, uint32_t *D, uint32_t D_size);

struct node *find_leaf(struct node *curr, int key);

int find_insert_pos(struct node *leaf, int key);

struct node *split_node(struct node *root,
                        struct node *node,
                        struct node **lo_result_node,
                        struct node **hi_result_node,
                        int *lo_hi_split_point,
                        void (*repair)(struct node *, struct node *));

void *bpt_find(struct node *root, struct node **leaf, int key);

void destroy_tree(struct node **root);

#endif
