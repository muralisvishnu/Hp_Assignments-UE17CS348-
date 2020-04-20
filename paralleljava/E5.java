import java.util.Random; 

public class E5 implements Runnable {
    private Random rand = new Random(System.currentTimeMillis());
    public void run() {
        for(int i=0; i<10000000; i++) {
            rand.nextInt();
        }
        System.out.println( Thread.currentThread().getName());
    }
    public static void main(String[] args) throws InterruptedException {
        Thread[] t = new Thread[5];        
        for(int i=0; i<t.length; i++) {
            t[i] = new Thread(new E5(), "thread-"+i);
            t[i].start();
        }
        for(int i=0; i<t.length; i++) {
            t[i].join();
        }
    }

}