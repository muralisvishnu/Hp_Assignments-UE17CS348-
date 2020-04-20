public class E1 {

    public static void main(String[] args) {

        long ID = Thread.currentThread().getId();
        int prior = Thread.currentThread().getPriority();
        String TGN = Thread.currentThread().getThreadGroup().getName();
        String name = Thread.currentThread().getName();
        System.out.println("ID = " + ID + "; name="+name+"; priority="+prior+"; ThreadgroupName="+TGN);

    }

}