#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TRUE (1==1)
#define FALSE (0==1)

static int node_count;

typedef struct node_s {
    int data;
    struct node_s *next;
} node_t;

typedef node_t *node;
typedef unsigned char bool;

node
mergelist(node i, node j)
{

   if (i == NULL) return j; 
   if (j == NULL) return i; 
   
   if (i->data < 
}
node
node_malloc()
{
    node t;
    t = (node) malloc(sizeof (node_t));
    if (t == NULL) {
        printf("\n No Memory Panic");
        assert(0);
    }
    t->next = NULL;
    t->data = node_count++;
    return (t);
}

print_list(node x)
{
    node tmp = x;

    if (x == NULL) {
        printf("\n NULL List \n");
    }

    for (; tmp; tmp = tmp->next) {
        printf("%d ", tmp->data);
    }
    printf("\n ");
}

bool
del_list(node * x, int del_data)
{
    node tmp,
      prev;

    if (*x == NULL) {
        printf("\n %s NULL list ", __FUNCTION__);
    }
    prev = *x;
    for (tmp = *x; tmp; prev = tmp, tmp = tmp->next) {
        if (tmp->data == del_data) {
            if (*x == tmp) {
                *x = tmp->next;
            } else {
                prev->next = tmp->next;
            }
            free(tmp);
            return (TRUE);
        }
    }
    return (FALSE);
}

node
add_list(node x, node add_ptr)
{
    if (x == NULL) {
        return (add_ptr);
    }

    add_ptr->next = x;
    x = add_ptr;

    return (x);
}

void
reverse_list(node * x)
{
    node tmp, prev, next;

    if (*x == NULL) {
        printf("\n %s: Null List\n", __FUNCTION__);
    }
    tmp = *x;
    prev = NULL;
    next = (*x)->next;

    for (; next->next; prev = tmp, tmp = next, next = next->next) {
        tmp->next = prev;
    }
    *x = tmp;

}

int
mid_list(node x)
{
    node mid;
    int i = 1;

    if (x == NULL) {
        printf("\n %s: NULL List", __FUNCTION__);
        return (-1);
    }

    mid = x;
    for (; x; x = x->next) {
        if ((i++ % 2) == 0) {
            mid = mid->next;
        }
    }
    return (mid->data);
}


main()
{
    node x, y, z, add_ptr;
    int i;

    x = node_malloc();

    for (i = 0; i < 10; i++) {
        add_ptr = node_malloc();
        x = add_list(x, add_ptr);
    }
    print_list(x);
    del_list(&x, 0);
    print_list(x);
    printf("\n List middle = %d\n", mid_list(x));
    del_list(&x, 10);
    print_list(x);
    printf("\n List middle = %d\n", mid_list(x));
    del_list(&x, 5);
    print_list(x);
    printf("\n List middle = %d\n", mid_list(x));

    reverse_list(&x);
    print_list(x);

    y = node_malloc();

    for (i = 0; i < 10; i++) {
        add_ptr = node_malloc();
        y = add_list(y, add_ptr);
    }
    print_list(y);

    mergelist(x,y);

}
