package ionic.Msmq;


public class Queue
{
  public final static int AccessReceive = 1;
  public final static int AccessSend = 2;
  public final static int AccessPeek = 32;
  public final static int AccessAdmin = 128;
  public final static int TimeoutInfinte = 0xFFFFFFFF;

  public Queue(String queueName)
    throws  MessageQueueException
  {
    _init(queueName, AccessReceive | AccessSend, false);	// open with both SEND and RECEIVE access
  }

  public Queue(String queueName, int access, boolean isTransactional)
    throws  MessageQueueException
  {
    _init(queueName, access, isTransactional); 
  }


  void _init(String queueName, int access, boolean isTransactional)
    throws  MessageQueueException
  {
	_queueHandles = new long[2];	// default values 0
    int rc = nativeOpenQueueWithAccess(_queueHandles, queueName, access);
	if (rc!=0)
		throw new  MessageQueueException("Cannot open queue.", rc);
		
	if (_queueHandles[0] == 0 && _queueHandles[1]==0)
		throw new  MessageQueueException("Failed to get queuehandle when opening queue.", rc);

    _name= queueName;
    _isTransactional= isTransactional;
  }


  public static void create(String queuePath, String queueLabel, boolean isTransactional) 
    throws  MessageQueueException
  {
    int rc= nativeCreateQueue( queuePath,  queueLabel, isTransactional);
    if (rc!=0) 
      throw new  MessageQueueException("Cannot create queue.", rc);
  }


  public static void delete(String queuePath)
    throws  MessageQueueException
  {
    int rc= nativeDeleteQueue( queuePath );
    if (rc!=0) 
      throw new  MessageQueueException("Cannot delete queue.", rc);
  }


  public void send(Message msg) 
    throws  MessageQueueException
  {
    int rc= nativeSend(_queueHandles,
                       msg,
                       true);
    if (rc!=0) 
      throw new MessageQueueException("Cannot send.", rc);
  }


  public void send(String s) 
    throws  MessageQueueException, java.io.UnsupportedEncodingException
  {
	send(new ionic.Msmq.Message(s, "", ""));
  }

  private ionic.Msmq.Message receiveEx(Message msg, int timeout, boolean peekOnly)
    throws  MessageQueueException
  {
	int rc = nativeReceive(_queueHandles, msg, _isTransactional, timeout, peekOnly);
	
	if (rc == -1072824293)	//MQ_ERROR_IO_TIMEOUT
       return null;

    if (rc!=0) 
      throw new MessageQueueException("Cannot receive.", rc);
	  
	return msg;
  }
  
  public ionic.Msmq.Message receive(Message msg, int timeout)
    throws  MessageQueueException
  {
	return receiveEx(msg, timeout, false);
  }
  
  public ionic.Msmq.Message receive(int timeout)
    throws  MessageQueueException
  {
  	Message msg = new Message();
	return receive(msg, timeout);
  }

  public ionic.Msmq.Message receive()
    throws  MessageQueueException
  {
    return receive(0);
  }

  public ionic.Msmq.Message peek(Message msg, int timeout)
    throws  MessageQueueException
  {
	return receiveEx(msg, timeout, true); 
  }

  public ionic.Msmq.Message peek(int timeout)
    throws  MessageQueueException
  {
    Message msg = new Message();
	return peek(msg, timeout); 
  }
  
  public ionic.Msmq.Message peek()
    throws  MessageQueueException
  {
    return peek(0);
  }

  public void close() 
    throws  MessageQueueException 
  {
    int rc=nativeClose(_queueHandles);
    if (rc!=0) 
      throw new MessageQueueException("Cannot close.", rc);
	_queueHandles = null;
  }


  // --------------------------------------------
  // getters on the Queue properties
  public String getName(){ return _name; }
  public boolean isTransactional(){ return _isTransactional; }

  // --------------------------------------------
  // native methods
  private static native int nativeCreateQueue(String queuePath, String queueLabel, boolean isTransactional);
  private static native int nativeDeleteQueue(String queuePath);
  private native int nativeOpenQueueWithAccess(long[] queueHandles, String queueString, int access);
  private native int nativeReceive(long[] queueHandles, ionic.Msmq.Message msg, boolean isTransactional, int timeout, boolean peekOnly);
  private native int nativeSend(long[] queueHandles, ionic.Msmq.Message msg, boolean isTransactional);
  private native int nativeClose(long[] queueHandles);

  // --------------------------------------------
  // private members
  String _name;
  boolean _isTransactional;
  long[] _queueHandles;

  // --------------------------------------------
  // static initializer
  static {
    System.loadLibrary("JNIMSMQ");
  }
}
