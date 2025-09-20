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


//Funcion que crea un nuevo TreeMap
/*Como: Reserva memoria para el nuevo TreeMap, inicializa las raíces y el puntero current
a NULL y guarda la funcion de comparacion (lower_than) para poder comparar las claves al
momento de decidir su orden el árbol
*/
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


//Funcion que inserta un nuevo par(key, value) en el árbol
/*Como: Si el árbol esta vacio, se crea un nuevo nodo con el par a ingresar y,
este se transforma en la raíz del árbol. En cambio, si no esta vacio, busca la posición 
correcta comparando claves con lower_than, e inserta el nodo como hijo izquierdo o derecho
según corresponda. Pero, si la clave ya existe en árbol, no la inserta de nuevo
*/
void insertTreeMap(TreeMap * tree, void* key, void * value) {
    if(tree == NULL){
        return;
    }

    //Si el árbol esta vacio
    if(tree -> root == NULL){
        tree -> root = createTreeNode(key, value);
        tree -> current = tree -> root;
        return;
    }

    //Si el árbol no esta vacio
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

    //Creamos un nodo new que almacena el nuevo par(key, value)
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


//Funcion para encontrar el elemento más pequeño en una rama
/*Como: Dado un nodo (padre), la funcion busca el hijo menor del nodo moviendose
siempre hacia la izquierda hasta que ya no se puede (llegó a un NULL)
*/
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


//Funcion que elimina un nodo del árbol, en tres casos distintos
/*Como: Si un nodo(padre) no tiene hijos, simplemente lo desconecta. Si el padre tiene un
hijo, se sustituye el nodo por su hijo y si este tiene dos hijos, se busca el sucesor 
inmediata(minimo del subárbol derecho) y se copian sus datos en el nodo a eliminar(padre), 
y luego se elimina el sucesor.
*/
void removeNode(TreeMap * tree, TreeNode* node) {
    if(node == NULL){
        return;
    }

    //Caso 1: Sin hijos
    if(node -> left == NULL && node -> right == NULL){
        if(node -> parent == NULL){
            tree -> root = NULL;
        }
        else{
            if(node -> parent -> left == node){
                node -> parent -> left = NULL;
            }
            else if(node -> parent -> right == node){
                node -> parent -> right = NULL;
            }
        }
        free(node);
        return;
    }

    //Caso 2: Un hijo
    if(node -> left == NULL || node -> right == NULL){
        TreeNode *child = (node -> left != NULL) ? node -> left : node ->right;

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
        child -> parent = node -> parent;
        free(node);
        return;
    }

    //Caso 3: Dos hijos
    TreeNode * minNode = minimum(node -> right);
    node -> pair -> key = minNode -> pair -> key;
    node -> pair -> value = minNode -> pair -> value;

    removeNode(tree, minNode);
}


//Funcion que elimina un par del árbol dado su key
/*Como: Busca el nodo con esa clave y luego llama a la función removeNode
*/
void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);
}


//Función que busca una clave en el árbol
/*Como: Busca un nodo que tenga exactamente la clave (key) empezando desde la raíz.
Si la encuentra (son iguales), devuelve el par (key, value). Si la clave buscada es menor
a la clave en la que esta baja a la izquierda pero si es mayor baja a la derecha. 
Por ultimo, si llega a NULL, significa que no existe esa clave en el árbol y devuelve NULL.
*/
Pair * searchTreeMap(TreeMap * tree, void* key) {
    if(tree == NULL || tree -> root == NULL){
        return NULL;
    }
    TreeNode * node = tree -> root;
    while(node != NULL){
        void* i = node -> pair -> key;
        if (is_equal(tree, i, key)){
            tree -> current = node;
            return node -> pair;
        }
        if(tree -> lower_than(key, i)){
            node = node -> left;
        }
        else{
            node = node -> right;
        }
    }
    return NULL;
}


//Función busca el menor elemento que sea mayor o igual a la clave dada
/*Como: Busca el menor nodo cuya clave sea mayor o igual a key. Si encuentra la clave
exacta, la devuelve pero si no, cada vez que baje por la izquierda, porque key < nodo -> key,
guarda ese nodo como posible candidato en ls_node.
Al final, el mejor candidato(ls_node) será el más pequeño que sigue siendo mayor o igual a key
*/
Pair * upperBound(TreeMap * tree, void* key) {
    if (tree == NULL || tree -> root == NULL){
        return NULL;
    }
    TreeNode * node = tree -> root;
    TreeNode * ls_node = NULL;

    while(node != NULL){
        void* i = node -> pair -> key;
        if(is_equal(tree, i, key)){
            tree -> current = node;
            return node -> pair;
        }
        if(tree -> lower_than(key, i)){
            ls_node = node;
            node = node -> left;
        }
        else{
            node = node -> right;
        }

    }
    if (ls_node != NULL){
        tree -> current = ls_node;
        return ls_node -> pair;
    }
    return NULL;
}

/*Funcion que devuelve el primer par del mapa(el nodo más a la izquierda del árbol) y
actualiza el current a ese nodo
*/
Pair * firstTreeMap(TreeMap * tree) {
    if(tree == NULL || tree -> root == NULL){
        return NULL;
    }
    TreeNode * min = minimum(tree -> root);
    if(min == NULL){
        return NULL;
    }
    tree -> current = min;
    return min -> pair;
}

//Funcion que devuelve el siguiente nodo en orden creciente(menor a mayor clave)
/*Como: Si el nodo actual tiene hijo derecho, el siguiente es el mínimo del subárbol derecho,
si el nodo actual no tiene hijo derecho, sube hacia los padres hasta encontrar el primero que sea 
mayor que el actual.
Si no hay siguiente, ya estamos en el último nodo y se devuelve NULL
*/
Pair * nextTreeMap(TreeMap * tree) {
    if(tree == NULL || tree -> current == NULL){
        return NULL;
    }
    TreeNode * node = tree -> current;
    if(node -> right != NULL){
        TreeNode *sig = minimum(node -> right);
        tree -> current = sig;
        return sig -> pair;
    }
    TreeNode * father = node -> parent;
    while(father != NULL && node == father -> right){
        node = father;
        father = father -> parent;
    }

    tree -> current = father;
    return (father == NULL) ? NULL: father -> pair;

    return NULL;
}
