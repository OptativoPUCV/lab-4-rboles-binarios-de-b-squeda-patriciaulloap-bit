#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * map = (TreeMap*) malloc(sizeof(TreeMap));
    if(map == NULL){
        return NULL;
    }
    map -> root = NULL;
    map -> current = NULL;
    //new->lower_than = lower_than;
    map -> lower_than = lower_than;
    return map;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {
    if(tree == NULL){
        return;
    }
    if(tree -> root == NULL){
        tree -> root = createTreeNode(key, value);
        tree -> current = tree -> root;
        return;
    }

    TreeNode *parent = NULL;
    TreeNode *node = tree -> root;
    while(node != NULL){
        parent = node;
        void* i = node -> pair -> key;
        if(is_equal(tree,i,key)){
            tree -> current = node;
            return;
        }
        if(tree -> lower_than(key, i)){
            node = node -> left;
        }
        else{
            node = node -> right;
        }
    }
    TreeNode * new = createTreeNode(key, value);
    new -> parent = parent;
    if(tree -> lower_than(key, parent -> pair-> key)){
        parent -> left = new;
    }
    else{
        parent -> right = new;
    }
    tree -> current = new;
}

TreeNode * minimum(TreeNode * x){
    if(x == NULL){
        return NULL;
    }
    TreeNode* nodo = x;
    while(nodo -> left != NULL){
        nodo = nodo -> left;
    }
    return nodo;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    if(tree == NULL || node == NULL){
        return;
    }
    if(node -> left != NULL && node -> right != NULL){
        TreeNode * sucesor = minimum(node -> right);
        free(sucesor -> pair);
        sucesor -> pair = node -> pair;
        node -> pair = NULL;

        removeNode(tree, sucesor);
        return;
    }

    TreeNode * child;
    if(node -> left != NULL){
        child = node -> left;
    }
    else{
        child = node -> right;
    }

    if(child != NULL){
        child -> parent = node -> parent;

    }
    if(node -> parent == NULL){
        tree -> root = child;
    }
    else{
        if(node == node -> parent -> left){
            node -> parent -> left = child;
        }
        else{
            node -> parent -> right = child;
        }
    }
    if(tree -> current == node){
        tree -> current = node -> parent;
    }
    if(node -> pair){
        free(node -> pair);
    }
    free(node);
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}




Pair * searchTreeMap(TreeMap * tree, void* key) {
    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) {
    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    return NULL;
}

Pair * nextTreeMap(TreeMap * tree) {
    return NULL;
}
