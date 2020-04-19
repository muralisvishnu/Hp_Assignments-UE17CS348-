#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
struct node {
   int d;
   int fibsum;
   struct node* link;
};
int fib(int num) {
   int i, j;
   if (num < 2) {
      return (num);
   } else {
      i = fib(num - 1);j = fib(num - 2);
	   return (i + j);
   }
}
void work(struct node* par) 
{
   int i;i=par->d;
   par->fibsum = fib(i);
}
struct node* init(struct node* par) {
    int i;
    struct node* h = NULL;struct node* t = NULL;
    h = malloc(sizeof(struct node));
    par = h;
    par->d = 32;
    par->fibsum = 0;
    for (i=0; i< 5; i++) {
       t  =  malloc(sizeof(struct node));
       par->link = t;par = t;par->d = 32 + i + 1;par->fibsum = i+1;
    }
    par->link = NULL;
    return h;
}
int main(int argc, char *argv[]) {
     struct node *p=NULL;struct node *t=NULL;struct node *h=NULL;
     p = init_list(p);
     h = p;
     struct node ** nodes = (struct node **)malloc(sizeof(struct node*));
     int count = 0;
     while (p != NULL) {
           nodes[count++] = p;
           nodes = (struct node **)realloc(nodes,sizeof(struct node*)*(count+1)); // make another pass instead. This is eipensive
		   p = p->link;
        }
    #pragma omp parallel
    {
        #pragma omp single
        {
            while (p != NULL) {
                #pragma omp task firstprivate(p)
                {
                work(p);
                }
                p = p->link;
            }
        }
    }
    p = h;
	  while (p != NULL) {
        printf("%d : %d\n",p->d, p->fibsum);
        t = p->link;
        free (p);
        p = t;
     }  
	  free (p);
     return 0;
}



