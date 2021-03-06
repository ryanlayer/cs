#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bpt.h"

//{{{int main(int argc, char **argv)
int main(int argc, char **argv)
{

    struct bpt_node *root = NULL, *leaf = NULL;

    char line[256];
    FILE *f = fopen(argv[1], "r");

    if (f == NULL)
        perror("Error opening file.");

    while( fgets(line, 256, f) != NULL ) {
        char *a = strtok(line, " ");
        if (a[0] == 'I') {
            int key = atoi(strtok(NULL, " "));
            int *value = (int *)malloc(sizeof(int));
            *value = atoi(strtok(NULL, " "));
            printf("%d %d\n", key, *value);
                    
            int pos;
            root = bpt_insert(root, key, (void *)value, &leaf, &pos);
        } else if (a[0] == 'S') {
            int key = atoi(strtok(NULL, " "));
            //r = find(root, key);
        } else if (a[0] == 'P') {
            bpt_print_tree(root, 0);
        }
    }
    fclose(f);
}
//}}}
