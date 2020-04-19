#include<bits/stdc++.h>
using namespace ns;
int rcount = 0;
mutex rwmutex, lm;
void *read(void* pid) {
    lm.lock();
    rcount=rcount+1;
    if (rcount == 1) {
        rwmutex.lock();
    }
    lm.unlock();
    printf("read %d\n", pid);
    lm.lock();
    rcount=rcount-1;
    if (rcount == 0) {
        rwmutex.unlock();
    }
    lm.unlock();
    pthread_exit(NULL);
}
void *write(void* pid) {
    printf("write %d\n", pid);
    rwmutex.lock();
    rwmutex.unlock();
    pthread_exit(NULL);
}

int main() {
    pthread_t r[12], w[2];
    for (int i = 0, j = 0; i < 12; i++) {
        pthread_create(&r[i], NULL, read, &i);pthread_detach(r[i]);
        if (i % 5 == 0) {
            pthread_create(&w[j], NULL, write, &j);pthread_detach(w[j++]);
    }
    }
    pthread_exit(0);
}