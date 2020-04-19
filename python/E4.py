import threading
import queue
import random
def p(q,lock):
    lock.acquire()
    i = random.randint(1,100)
    q.put(random.randint(1,100))
    print(threading.currentThread().getName(),"p",i)
    lock.release()
def c(q,lock):
    r = 0
    while (not r):
        lock.acquire()
        if (not q.empty()):
            r = 1
            print(threading.currentThread().getName(),"c",q.get())
        lock.release()
t = []
q = queue.Queue()
lock = threading.Lock()
for i in range(1,12):
    if (i %2):
        t.append(threading.Thread(None, p,i,(q,lock)))
    else:
        t.append(threading.Thread(None,c,i,(q,lock)))

for i in t:
    i.start()

for i in t:
    i.join()