import java.util.Random;
public class E7 {
    private static int account = 0;
    public static class Withdraw implements Runnable {
        public synchronized void run() {
            Random rand = new Random();
                if(account < 1000) {
                    System.out.println("Below Balance");
                }
                else {
                    account -= rand.nextInt(1000);
                    System.out.println( account);
                }
        }
    }
    public static class Deposit implements Runnable{
        Random rand = new Random();
        public synchronized void run() {
                account += rand.nextInt(1000);System.out.println( account);
        }
    }
    public static void main(String[] args) throws InterruptedException {
        Thread[] ws  = new Thread[5];
        Thread[] d = new Thread[6];
        for(int i = 0; i < ws.length; i++)
        {
            ws[i] = new Thread(new Withdraw(), "w-"+i);
        }
        for(int i = 0; i < d.length; i++)
        {
            d[i] = new Thread(new Deposit(), "d-"+i);
        }
        for(int i = 0; i < ws.length; i++)
        {
            d[i].join();
        }
        for(int i = 0; i < d.length; i++)
        {
            d[i].join();
        }
        d[5].start();d[0].start();ws[0].start();d[1].start();ws[1].start();d[2].start();ws[2].start();d[3].start();ws[3].start();d[4].start();ws[4].start();
    }
}
