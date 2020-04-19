import threading
B = 500
lock = threading.Lock()
def updatebalance(c):
    lock.acquire()
    global B
    if (B + c < 1000):
        print(threading.currentThread().getName(),"Minimum balance")
    else:
        print(threading.currentThread().getName(),B)
        B = B + c
        print(threading.currentThread().getName(),B) 
    lock.release()
t = []
for i in range(1,11):
    if (i %2):
        t.append(threading.Thread(None,updatebalance,i,(-100,)))
    else:
        t.append(threading.Thread(None,updatebalance,i,(100,)))    
for i in t:
    i.start()