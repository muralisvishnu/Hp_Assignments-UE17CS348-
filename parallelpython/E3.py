from queue import PriorityQueue
import threading
import random
import time
class MTQ:
    def __init__(self, q, lock):
        self.q = q
        self.lock = lock
    def push(self, val):
        self.lock.acquire()
        self.q.put(val)
        self.lock.release()
        print("ADD", val)
    def pop(self):
        self.lock.acquire()
        if not self.q.empty():
            data = self.q.pop()
            print("REMOVE", data)
        self.lock.release()
def p(Q):
     Q.push((random.randint(), random.randint()))
     time.sleep(2)
def c(Q):
    Q.pop()
lock = threading.Lock()
q = MPQ(PriorityQueue(), lock)
t = []
for i in range(1,12):
    if (i %2):
        t.append(threading.Thread(None, q.add,i,(random.randint(1,100),)))
    else:
        t.append(threading.Thread(None,q.get,i))

for i in t:
    i.start()

for i in t:
    i.join()