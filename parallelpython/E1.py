import threading
import time
def function(n,d):
    c = 0
    while c < 20:
        c += 1
        print(n,c,d)
t1 = threading.Thread(None,function,"Thread1",("Thread1",1))
t2 = threading.Thread(None,function,"Thread2",("Thread2",2))
t1.start()
t2.start()