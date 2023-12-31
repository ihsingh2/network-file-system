#include "defs.h"
#include "list.h"

void list_init(list_t* L)
{
  L->head = (snode_t*) malloc(sizeof(snode_t));
  L->head->st.ip[0] = 0;
  L->head->st.nsport = -1;
  L->head->st.clport = -1;
  L->head->st.stport = -1;
  L->head->prev = L->head;
  L->head->next = L->head;
  L->last = L->head;
  pthread_mutex_init_tx(&L->lock, NULL);
}

snode_t* list_insert(list_t* L, storage_t st)
{
  if (list_search(L, st) != 0)
    return NULL;

  snode_t* x = (snode_t*) malloc(sizeof(snode_t));
  x->st = st;
  x->down = 0;

  pthread_mutex_lock_tx(&L->lock);
  L->num++;
  L->head->prev->next = x;
  x->prev = L->head->prev;
  L->head->prev = x;
  x->next = L->head;
  pthread_mutex_unlock_tx(&L->lock);

  return x;
}

void list_delete(list_t *L, snode_t* x)
{
  if (L->head == x)
    return;

  pthread_mutex_lock_tx(&L->lock);
  x->prev->next = x->next;
  x->next->prev = x->prev;
  pthread_mutex_unlock_tx(&L->lock);

  free(x);
}

snode_t* list_search(list_t* L, storage_t st)
{
  pthread_mutex_lock_tx(&L->lock);
  snode_t* node = L->head->next;

  while(node != L->head) {
    if (stequal(node->st, st)) {
      pthread_mutex_unlock_tx(&L->lock);
      return node;
    }
    node = node->next;
  }

  pthread_mutex_unlock_tx(&L->lock);
  return NULL;
}

snode_t* list_random(list_t* L)
{
  snode_t* node = L->last->next;
  while (node != L->last) {
    if (node == L->head)
      node = node->next;
    else {
      L->last = node;
      return node;
    }
  }
  logns(FAILURE, "list_random: reached the end of list");
  return NULL;

  /*
  int i;
  int hops;
  snode_t* node;

  srand(time(NULL));
  hops = rand() % L->num;

  i = 0;
  node = L->head->next;
  while (node != L->head) {
    if (i == hops)
      return node;
    i++;
    node = node->next;
  }

  logns(FAILURE, "list_random: reached the end of list");
  return NULL;
  */
}

int stequal(storage_t a, storage_t b)
{
  if (strcmp(a.ip, b.ip) == 0 && a.nsport == b.nsport && a.clport == b.clport && a.stport == b.stport)
    return 1;

  return 0;
}
