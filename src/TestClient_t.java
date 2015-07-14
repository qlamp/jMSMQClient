import ionic.Msmq.Queue;
import ionic.Msmq.Message;
import ionic.Msmq.MessageQueueException;

public class TestClient_t 
{
  Queue Msmq= null;

  String ipAddr= " [ip:?]"; 
  public TestClient_t() {
    try {
    java.net.InetAddress thisIp = java.net.InetAddress.getLocalHost();
    ipAddr= " [ip:"+thisIp.getHostAddress() + "]"; 
    }catch (Exception ex1) {
    }
  }


  public static void main (String [] args) throws java.lang.Exception 
  {
    TestClient_t c= new TestClient_t();
    c.Run();
  }

  private void Receive() {
    try {
      checkOpen();
      System.out.println("receive");
      Message msg= Msmq.receive(2000); // timeout= 2000 ms 
      System.out.println(" ==> message: " + msg.getMessage());
      System.out.println("     label:   " + msg.getLabel());
    }
    catch (Exception ex1) {
      System.out.println("Receive failure: " + ex1);
    }
  }

  private void Peek() {
    try {
      checkOpen();
      System.out.println("peek");
      Message msg= Msmq.peek(2000); // timeout= 2000 ms 
      System.out.println(" ==> message: " + msg.getMessage());
      System.out.println("     label:   " + msg.getLabel());

    }
    catch (Exception ex1) {
      System.out.println("Peek failure: " + ex1);
    }
  }

  private void Close() {
    try {
      checkOpen();
      System.out.println("close");
      Msmq.close();
      Msmq= null;
      System.out.println("close: OK.");
    }
    catch (MessageQueueException ex1) {
      System.out.println("close failure: " + ex1);
    }
  }


  private void Send() {
    try {
      checkOpen();
      // the transaction flag must agree with the transactional flavor of the queue. 
      String mLabel="inserted by " + this.getClass().getName() + ".java" ;

      String correlationID= "L:none";
      java.util.Calendar cal= java.util.Calendar.getInstance(); // current time & date
      java.text.SimpleDateFormat df = new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:sszzz");
      String message= "[from:Java] [time:" +    df.format(cal.getTime()) + "]" + ipAddr; 
      System.out.println("send (" + message + ")");

      Message msg= new Message(message, mLabel, correlationID);

	  Msmq.send(msg); 
    }
    catch (Exception ex1) {
      System.out.println("Send failure: " + ex1);
    }
  }


  private void Create() 
  {
    try {
      String qname= getLine("\nName of Queue to create? ");

      // I think you can only create locally... Thu, 21 Apr 2005  18:43
      //String hostname= getLine("\nName of Queue Server (ENTER for local)? ", false);
      //String fullname= getQueueFullName(hostname,qname);
      String fullname= ".\\private$\\" + qname; 
      System.out.println("create (" + fullname + ")");
      String qLabel="Created by " + this.getClass().getName() + ".java";
      boolean transactional= true;  // should the queue be transactional
      Queue.create(fullname, qLabel, transactional);
      System.out.println("Create: OK.");
    }
    catch (MessageQueueException ex1) {
      System.out.println("Queue creation failure: " + ex1);
    }
  }


  private void Delete() 
  {
    try {
      String qname= getLine("\nName of Local Queue to Delete? ");
      String fullname= getQueueFullName(".",qname);
      System.out.println("delete (" + fullname + ")");
      ionic.Msmq.Queue.delete(fullname);
      System.out.println("delete: OK.");
    }
    catch (MessageQueueException ex1) {
      System.out.println("Queue deletion failure: " + ex1);
    }
  }


  private void Open() 
  {
    try {
      if (Msmq!=null) {
	Msmq.close();
	Msmq= null;
      }
      String qname= getLine("\nName of Queue to open? ");
      String hostname= getLine("\nName of Queue Server (ENTER for local)? ", false);
      String fullname= getQueueFullName(hostname,qname);
      System.out.println("open (" + fullname + ")");
      Msmq= new Queue(fullname); 
      //Msmq= new Queue(fullname, 0x02); // 0x02 == SEND only
      System.out.println("open: OK.");
    }
    catch (MessageQueueException ex1) {
      System.out.println("Queue open failure: " + ex1);
    }
  }
  
  private void AutomaticTest()
  {
	try
	{
		TestRunner testRunner = new TestRunner();
		testRunner.testQueues();
		testRunner.testTransactionalQueues();
		testRunner.testTransactionalQueues();
		testRunner.testBinaryMessages();
		System.out.println("\nAutomatic Test Passed\n");
	}
	catch(Exception ex1)
	{
      System.out.println("Exception: " + ex1);
      ex1.printStackTrace();
	  System.out.println("\nAutomatic Test failed!!!\n");
	}
  }

  private String getQueueFullName( String queueShortName ) {
    return getQueueFullName(".", queueShortName);
  }

  private String getQueueFullName( String hostname, String queueShortName ) {
    String h1= hostname; 
    String a1= "OS";
    if ((h1==null) || h1.equals("")) h1="."; 
    char[] c= h1.toCharArray();
    if ((c[0]>='1')
	&& (c[0]<='9')) a1= "TCP";

    return "DIRECT=" + a1 + ":" + h1 + "\\private$\\" + queueShortName;  
  }

  private String getLine(String prompt) {
    return getLine(prompt, true);
  }
  private String getLine(String prompt, boolean discardPending) {
    byte[] b= new byte[256];
    int n=0;
    try {
      if (discardPending)
	clearInput();
      System.out.print(prompt);
      n= System.in.read(b); // read one line
    }
    catch(Exception e1) {
      System.out.print("\nExc!!\n");
      e1.printStackTrace();
    }
    String result= new java.lang.String(b).trim(); 
    return result;
  }

  private void clearInput() 
throws java.io.IOException {
    int c;
    do {
      c= System.in.read(); // read one char at a time
    }
    while (c!=10);
  }

  private void checkOpen() 
  throws MessageQueueException {
    if (Msmq==null) 
      throw new MessageQueueException("open a queue first!\n",-1);
  }


  private void Info() {
    System.out.print("\nHello from the Java MSMQ Test Client.\n");

    System.out.println("  class name: " + this.getClass().getName() );
    String[] propNames= {
      "java.version", 
      "java.vendor", 
      "java.class.version", 
      "java.vm.version", 
    };

    String p;
    for (int i = 0 ; i < propNames.length; i++ ) {
      p= java.lang.System.getProperty(propNames[i]); 
      System.out.println("  " + propNames[i] + ": " + p );
    }
  }

  
  public void Run() 
    throws java.lang.Exception {
	
    try {
      System.out.print("\nHello from the Java MSMQ Test Client.\n");

      int state= 1;
      do {
	// only prompt after processing a command. 
	if (state==1) {
	  System.out.print("\n[C]reate [O]pen [R]eceive [P]eek [S]end [X]close [D]elete [I]nfo or [Q]uit?\n==> ");
	  state=0;
	}
	int c= System.in.read(); // read one char at a time
	if ((c=='C')||(c=='c'))      { Create();state=1;}
	else if ((c=='D')||(c=='d')) { Delete(); state=1;}
	else if ((c=='O')||(c=='o')) { Open(); state=1; }
	else if ((c=='R')||(c=='r')) Receive(); 
	else if ((c=='S')||(c=='s')) Send(); 
	else if ((c=='I')||(c=='i')) Info(); 
	else if ((c=='X')||(c=='x')) Close(); 
	else if ((c=='P')||(c=='p')) Peek(); 
	else if ((c=='T')||(c=='t')) AutomaticTest(); 
	else if ((c=='Q')||(c=='q')) state=99;
	else if (c==13) state= 1;  // ENTER
      } while (state!=99);
 
      if (Msmq!=null) Msmq.close();
      System.out.println("\n");
    }

    catch (Exception ex1) {
      System.out.println("Exception: " + ex1);
      ex1.printStackTrace();
    }
  }
}
