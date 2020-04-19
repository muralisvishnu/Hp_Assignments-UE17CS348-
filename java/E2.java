
public class E2 extends Thread {

    public E2(String n) {
        super(n);
    }

    public static void main(String[] args) {
        E2 t1 = new E2("1");E2 t2 = new E2("2");E2 t3 = new E2("3");E2 t4 = new E2("4");E2 t5 = new E2("5");
        t1.start();t2.start();t3.start();t4.start();t5.start();
    }

    @Override
    public void run() {
        System.out.println(Thread.currentThread().getName());
    }

}