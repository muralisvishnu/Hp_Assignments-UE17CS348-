#include <pthread.h>
#include <semaphore.h>
#include <iostream>
void* add(void*);
void* subtract(void*);
using namespace std;
int buffer[1000] = {0};
typedef struct vars{
  sem_t *e;
  sem_t *f;
  sem_t *m;
  int n;
}vars;
void* add(void* pars){
  vars *p = (vars*)pars;
  sem_t *e = p->e;
  sem_t *f = p->f;
  sem_t *m = p->m;
  int n = p->n;
  int j;
  for(int i=0; i<n; i++){
    sem_wait(e);
    sem_wait(m);
    sem_getvalue(f, &j);
    buffer[j] = 1;
    sem_post(m);
    sem_post(f);
  }
}
void* subtract(void* pars){
  vars *p = (vars*)pars;
  sem_t *e = p->e;
  sem_t *f = p->f;
  sem_t *m = p->m;
  int n = p->n;
  int j;
  for(int i=0; i<n; i++){
    sem_wait(f);
    sem_wait(m);
    sem_getvalue(f, &j);
    buffer[j] = 0;
    sem_post(m);
    sem_post(e);
  }
}
int main(){
  sem_t e;sem_t f;sem_t m;
  sem_init(&e, 0, 1000);sem_init(&f, 0, 0);sem_init(&m, 0, 1);
  vars *producer = (vars*)malloc(sizeof(vars));vars *costdumer = (vars*)malloc(sizeof(vars));
  producer->e = &e;producer->f = &f;producer->m = &m;
  costdumer->e = &e;costdumer->f = &f;costdumer->m = &m;
  producer->n = 200;costdumer->n = 50;
  pthread_t p, c;
  pthread_create(&p,NULL,add,(void*)producer);pthread_create(&c,NULL,subtract,(void*)costdumer);
  pthread_join(p, NULL);pthread_join(c, NULL);
  for(int i=0; i<1000; i++){
    cout << buffer[i];
  }
  cout << endl;
  return 0;
}
