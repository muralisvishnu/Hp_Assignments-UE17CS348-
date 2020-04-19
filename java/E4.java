
public class E4 implements Runnable{

    public void run() {
        try {
            Thread.sleep(Long.MAX_VALUE);
        } catch (InterruptedException e) {
            System.out.println(("error :" + Thread.currentThread().getName()));
        }
        while(!Thread.interrupted());
        System.out.println(Thread.currentThread().getName());
    }

    public static void main(String[] args) throws InterruptedException{
        Thread t1 = new Thread(new E4(), "t1");
        t1.start();
        System.out.println( Thread.currentThread().getName() + "sleeps 3s");
        Thread.sleep(3000);
        System.out.println(Thread.currentThread().getName() + " interrupted");
        t1.interrupt();
        System.out.println( Thread.currentThread().getName() + "sleeps 3s");
        Thread.sleep(3000);
        System.out.println( Thread.currentThread().getName() + " interrupted");
        t1.interrupt();
    }

}