#include<iostream> 
#define n 4 
using namespace ns;
int compltedPhilo = 0,i; 
struct fork{
	int taken;
}fork[n]; 
struct philosp{
	int left;
	int right;
}philosopher[n]; 
void goForDinner(int pID){ 
	if(philosopher[pID].left==10 && philosopher[pID].right==10)
        cout<<"Philosopher "<<pID+1<<" completed his dinner\n";
	else if(philosopher[pID].left==1 && philosopher[pID].right==1){
            cout<<"Philosopher "<<pID+1<<" completed his dinner\n";
 
            philosopher[pID].left = philosopher[pID].right = 10; 
            int otherFork = pID-1;
            if(otherFork== -1)
                otherFork=(n-1);
            fork[pID].taken = fork[otherFork].taken = 0;
            cout<<"Philosopher "<<pID+1<<" released fork "<<pID+1<<" and fork "<<otherFork+1<<"\n";
            compltedPhilo++;
        }
        else if(philosopher[pID].left==1 && philosopher[pID].right==0){
                if(pID==(n-1)){
                    if(fork[pID].taken==0){ 
                        fork[pID].taken = philosopher[pID].right = 1;
                        cout<<"Fork "<<pID+1<<" taken by philosopher "<<pID+1<<"\n";
                    }else{
                        cout<<"Philosopher "<<pID+1<<" is waiting for fork "<<pID+1<<"\n";
                    }
                }else{
                    int dpID = pID;
                    pID-=1;
 
                    if(pID== -1)
                        pID=(n-1);
 
                    if(fork[pID].taken == 0){
                        fork[pID].taken = philosopher[dpID].right = 1;
                        cout<<"Fork "<<pID+1<<" taken by Philosopher "<<dpID+1<<"\n";
                    }else{
                        cout<<"Philosopher "<<dpID+1<<" is waiting for Fork "<<pID+1<<"\n";
                    }
                }
            }
            else if(philosopher[pID].left==0){ 
                    if(pID==(n-1)){
                        if(fork[pID-1].taken==0){ 
                            fork[pID-1].taken = philosopher[pID].left = 1;
                            cout<<"Fork "<<pID<<" taken by philosopher "<<pID+1<<"\n";
                        }else{
                            cout<<"Philosopher "<<pID+1<<" is waiting for fork "<<pID<<"\n";
                        }
                    }else{
                        if(fork[pID].taken == 0){
                            fork[pID].taken = philosopher[pID].left = 1;
                            cout<<"Fork "<<pID+1<<" taken by Philosopher "<<pID+1<<"\n";
                        }else{
                            cout<<"Philosopher "<<pID+1<<" is waiting for Fork "<<pID+1<<"\n";
                        }
                    }
        }else{}
}
 
int main(){
	for(i=0;i<n;i++)
        fork[i].taken=philosopher[i].left=philosopher[i].right=0;
 
	while(compltedPhilo<n){
		for(i=0;i<n;i++)
            goForDinner(i);
		cout<<"\nCompleted Philosophers "<<compltedPhilo<<"\n\n";
	}
 
	return 0;
}