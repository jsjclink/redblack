#include<stdio.h>
#include<stdlib.h>

#define MAXVALSIZE 4
enum {RED = 1, BLACK = 0};

typedef struct Node{
  int key, val;
  int color;
  struct Node *left, *right, *parent;
}Node;

typedef struct LLRB{
  struct Node* root;
}LLRB;

Node* rotateLeft(Node* h){
  Node* x = h->right;
  h->right = x->left;
  x->left = h;
  x->color = h->color;
  h->color = RED;
  return x;
}
Node* rotateRight(Node* h){
  Node* x = h->left;
  h->left = x->right;
  x->right = h;
  x->color = h->color;
  h->color = RED;
  return x;
}
Node* flipColor(Node* h){
  h->color = h->color == RED ? BLACK : RED;
  h->left->color = h->left->color == RED ? BLACK : RED;
  h->right->color = h->right->color == RED ? BLACK : RED;
}
int isRed(Node* x){
  if(x == NULL) return 0;
  return x->color;
}

LLRB* createLLRB(){
  LLRB* new_llrb = (LLRB*)malloc(sizeof(LLRB));
  new_llrb->root = NULL;
  return new_llrb;
}
int get(LLRB* t, int key){
  Node* x = t->root;
  while(x != NULL){
    if      (key < x->key) x = x->left;
    else if (key > x->key) x = x->right;
    else                  return x->val;
  }
  return -1;
}
int getH(Node* x, int key){ //delete에서 쓰기 위해
  while(x != NULL){
    if      (key < x->key) x = x->left;
    else if (key > x->key) x = x->right;
    else                  return x->val;
  }
}
Node* putH(Node* h, int key, int val){
  if(h == NULL){
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->key = key;
    new_node->val = val;
    new_node->color = RED;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
  }
  if      (key < h->key) h->left = putH(h->left, key, val);
  else if (key > h->key) h->right = putH(h->right, key, val);
  else                   h->val = val;

  if(isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
  if(isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
  if(isRed(h->left) && isRed(h->right)) flipColor(h);

  return h;
}
/*
Node* root = t->root;
root = putH(root, key, val);
root->color = Black;
이런식으로 하면 안됨
return 받은 root 값이 t->root에 저장되는게 아니라 root라는 지역변수에 저장되기 때문
*/
void put(LLRB* t, int key, int val){
  t->root = putH(t->root, key, val);
  t->root->color = BLACK;
}


//delete
Node* fixUp(Node* h){
  if(isRed(h->right)) h = rotateLeft(h);
  if(isRed(h->left) && isRed(h->left->left))
    h = rotateRight(h);
  if(isRed(h->left) && isRed(h->right)) flipColor(h);
  return h;
}

Node* moveRedRight(Node* h){
  flipColor(h);
  if(isRed(h->left->left)){
    h = rotateRight(h);
    flipColor(h);
  }
  return h;
}
Node* deleteMaxH(Node* h){
  if(isRed(h->left)) h = rotateRight(h);
  /*h->right가 NULL이면 h->left또한 NULL임을 알 수 있음(위에서 rotation을 했음에도 불구하고 h->right가 NULL이니깐 애초에 딱 h만 있었다는거)*/
  if(h->right == NULL){
    free(h);
    return NULL;
  }
  if(!isRed(h->right) && !isRed(h->right->left)) h = moveRedRight(h);
  h->right = deleteMaxH(h->right);
  return fixUp(h);
}
void deleteMax(LLRB* t){
  if(t->root == NULL) return;
  if(!isRed(t->root->left) && !isRed(t->root->right)) t->root->color = RED;
  t->root = deleteMaxH(t->root);
  if(t->root) t->root->color = BLACK;
}

Node* moveRedLeft(Node* h){
  flipColor(h);
  if(isRed(h->right->left)){
    h->right = rotateRight(h->right);
    h = rotateLeft(h);
    flipColor(h);
  }
  return h;
}
Node* deleteMinH(Node* h){
  if(h->left == NULL){
    free(h);
    return NULL;
  }
  if(!isRed(h->left) && !isRed(h->left->left)) h = moveRedLeft(h);
  h->left = deleteMinH(h->left);
  return fixUp(h);
}
void deleteMin(LLRB* t){
  if(t->root == NULL) return;
  if(!isRed(t->root->left) && !isRed(t->root->right)) t->root->color = RED;
  t->root = deleteMinH(t->root);
  if(t->root) t->root->color = BLACK;
}

int min(Node* h){
  while(h->left != NULL)
    h = h->left;
  return h->key;
}

Node* deleteH(Node* h, int key){
  if(key < h->key){
    if(!isRed(h->left) && isRed(h->left->left)) h = moveRedLeft(h);
    h->left = deleteH(h->left, key);
  }
  else{
    if(isRed(h->left)) h = rotateRight(h);
    if(key == h->key && h->right == NULL){
      free(h);
      return NULL;
    }
    if(!isRed(h->right) && !isRed(h->right->left)) h = moveRedRight(h);
    if(key == h->key){
      h->key = min(h->right);
      h->val = getH(h->right, h->key);
      h->right = deleteMinH(h->right);
    }
    else h->right = deleteH(h->right, key);
  }
  return fixUp(h);
}
void delete(LLRB* t, int key){
  if(get(t, key) == -1) return;
  if(t->root == NULL) return;
  if(!isRed(t->root->left) && !isRed(t->root->right)) t->root->color = RED;
  t->root = deleteH(t->root, key);
  if(t->root) t->root->color = BLACK;
}



//util
void printTreeH(Node* h, int has_rchild[], int d){
  if(h == NULL) return;
  for(int i = 0; i < d-1; i++){
    if(has_rchild[i]){
      printf(" ");
    }
    else{
      printf("  ");
    }
  }
  printf("%d %s\n", h->key, h->color == RED ? "R" : "B");

  has_rchild[d] = 1;
  printTreeH(h->right, has_rchild, d+1);
  has_rchild[d] = 0;
  printTreeH(h->left, has_rchild, d+1);
}
void printTree(LLRB* t){
  if(t->root == NULL) return;
  printf("%d %s\n", t->root->key, t->root->color == RED ? "R" : "B");

  int* has_rchild = calloc(100,sizeof(int));
  has_rchild[0] = 1;
  printTreeH(t->root->right, has_rchild, 1);
  has_rchild[0] = 0;
  printTreeH(t->root->left, has_rchild, 1);
}



int main(){
  LLRB *llrb = createLLRB();
  printf("Created LLRB\n");
  while(1){
    char operation;
    printf("Which operation?(g-get p-put d-delete s-show M-deleteMax m-deleteMin) : ");
    scanf("%c", &operation);
    getchar();
    if(operation == 'g'){
      int key;
      printf("Key : ");
      scanf("%d", &key);
      getchar();
      printf("Key <=> Value : %d <=> %d\n", key, get(llrb, key));
    }
    else if(operation == 'p'){
      int key, val;
      printf("Key Val : ");
      scanf("%d %d", &key, &val);
      getchar();
      put(llrb, key, val);
    }
    else if(operation == 'd'){
      int key;
      printf("Key : ");
      scanf("%d", &key);
      getchar();
      delete(llrb, key);
    }
    else if(operation == 'M'){
      deleteMax(llrb);
    }
    else if(operation == 'm'){
      deleteMin(llrb);
    }
    else if(operation == 's'){
      printTree(llrb);
    }
  }
}