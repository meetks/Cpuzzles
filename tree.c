#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct node_s {
    int data;
    struct node_s *left;
    struct node_s *right;
} node_t;

node_t *
node_alloc(int x)
{
    node_t *temp;

    temp = (node_t *) malloc(sizeof (node_t));
    if (temp == NULL) {
        printf("\n Out of memory\n");
        assert(0);
    }
    memset(temp, 0, sizeof (node_t));
    temp->data = x;
    return (temp);
}

void 
tree_nuke(node_t *tree)
{
    if (tree) {
        tree_nuke(tree->left);
        tree_nuke(tree->right);
        free(tree);
    }
}

int
tree_destroy(node_t * tree)
{
    if (tree == NULL) {
        printf("\n Null tree cannot destroy\n");
        return (-1);
    }
    tree_nuke(tree);
}

node_t *
tree_create(int x)
{
    node_t *temp = NULL;
    temp = node_alloc(x);
    if (temp == NULL) {
        printf("\n %s: Something is very wrong\n", __FUNCTION__);
    }
    return (temp);
}

int
tree_add(node_t * node, int x)
{
    if (node == NULL) {
        printf("\n %s: The tree is empty \n", __FUNCTION__);
        return (-1);
    }

    if (x < node->data) {
        if (node->left == NULL) {
            node->left = node_alloc(x);
        } else {
            tree_add(node->left, x);
        }
    } else if (x > node->data) {
        if (node->right == NULL) {
            node->right = node_alloc(x);
        } else {
            tree_add(node->right, x);
        }
    }
}

tree_inorder(node_t * tree)
{
    if (tree != NULL) {
        tree_inorder(tree->left);
        printf("\n %d", tree->data);
        tree_inorder(tree->right);
    }
}

tree_postorder(node_t *tree)
{
    if (tree != NULL) {
        tree_inorder(tree->left);
        tree_inorder(tree->right);
        printf("\n %d", tree->data);
    }
}

tree_preorder(node_t *tree)
{
    if (tree != NULL) {
        printf("\n %d", tree->data);
        tree_inorder(tree->left);
        tree_inorder(tree->right);
    }
}

tree_kth_max(node_t *tree, int k)
{
    static int count = 0;
    if (tree) {
        tree_kth_max(tree->right,k);
        if (count++ == k) {
            printf("%d", tree->data);
        }
        tree_kth_max(tree->left,k);
    }
}

main()
{
    node_t *temp;
    int data;

    temp = tree_create(2);
    if (temp == NULL) {
        printf("\n Major Error\n");
        assert(0);
    }

    while (1) {
        printf("\n Enter the value :");
        scanf("%d", &data);
        if (data == -1) {
            break;
        }
        tree_add(temp, data);
    }
    tree_inorder(temp);
}
