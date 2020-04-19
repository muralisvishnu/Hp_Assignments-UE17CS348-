public class E3 implements Runnable {

    public void run() {
        System.out.println(Thread.currentThread().getName());
    }

    public static void main(String[] args) {

        Thread t1 = new Thread(new E3(), "t1");Thread t2 = new Thread(new E3(), "t2");Thread t3 = new Thread(new E3(), "t3");Thread t4 = new Thread(new E3(), "t4");Thread t5 = new Thread(new E3(), "t5");
        t1.start();t2.start();t3.start();t4.start();t5.start();
    }

}