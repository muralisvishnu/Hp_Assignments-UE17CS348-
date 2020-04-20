#include <pthread.h>
#include <iostream>
#include <semaphore.h>
using namespace std;
void* b(void*);
void* c(void*);
typedef struct vars{
  sem_t* c;
  sem_t* b;
  sem_t* m;
  int* fs;
}vars;
void* barber(void* par){
  vars *p = (vars*)par;
  sem_t* c = p->c;
  sem_t* b = p->b;
  sem_t* m = p->m;
  int* fs = p->fs;

  while(true){
    sem_wait(c);
    sem_wait(m);
    (*fs)=(*fs)+1;
    sem_post(m);
    sem_post(b);
  }
}

void* customer(void* par){
  vars *p = (vars*)par;
  sem_t* c = p->c;
  sem_t* b = p->b;
  sem_t* m = p->m;
  int* fs = p->fs;

  while(true){
    sem_wait(m);
    if(*fs > 0){
      (*fs)=(*fs)-1;
      sem_post(m);
      sem_post(c);
      sem_wait(b);
    }
    else{
      sem_post(m);
    }
  }
}

int main(){
  sem_t c, b, m;
  sem_init(&b,0, 0);
  sem_init(&c,0, 0);
  sem_init(&m, 0, 1);
  int fs = 15;
  pthread_t bp, cp;
  vars* p = (vars*)malloc(sizeof(vars));
  p->c = &c;
  p->b = &b;
  p->m = &m;
  p->fs = &fs;
  pthread_create(&bp, NULL, barber, (void*)p);
  pthread_create(&cp, NULL, customer, (void*)p);
  pthread_join(bp, NULL);
  pthread_join(cp, NULL);
  return 0;
}
