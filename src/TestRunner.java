import ionic.Msmq.Queue;
import ionic.Msmq.Message;
import ionic.Msmq.MessageQueueException;
import java.io.OutputStream;

public class TestRunner {

	private final String createQueuename = ".\\private$\\MsmqJavaTestRunner";
	private final String queuename = "DIRECT=OS:" + createQueuename;

	//@Test
	public void testQueues()
		throws Exception
	{
		try
		{
			Queue.delete(queuename);
		}
		catch(MessageQueueException ex)
		{}

		Queue.create(createQueuename, "MsmqJavaTestRunner", false);

		{
			Queue recvQueue = new Queue(queuename, Queue.AccessReceive, false); 
			Queue sendQueue = new Queue(queuename, Queue.AccessSend, false);
			String msg = "Hello";
			sendQueue.send(msg);
			Message result = recvQueue.receive();
			if (!msg.equals( result.getMessage() ))
				throw new Exception("Failed to receive message");
			if (result.getMessageType() != Message.VT_BSTR)
				throw new Exception("Failed to receive message type");
				
			recvQueue.close();
			sendQueue.close();
		}

		Queue.delete(queuename);
		
		try
		{
			Queue recvQueue = new Queue(queuename, Queue.AccessReceive, false);
			throw new Exception("Failed to delete queue");
		}
		catch(MessageQueueException ex)
		{
		}
	}
	
	//@Test
	public void testTransactionalQueues()
		throws Exception
	{
		try
		{
			Queue.delete(queuename);
		}
		catch(MessageQueueException ex)
		{}
		Queue.create(createQueuename, "MsmqJavaTestRunner", true);

		{
			Queue recvQueue = new Queue(queuename, Queue.AccessReceive, true); 
			Queue sendQueue = new Queue(queuename, Queue.AccessSend, true);
			String msg = "Hello";
			sendQueue.send(msg);
			Message result = recvQueue.receive();
			if (!msg.equals( result.getMessage() ))
				throw new Exception("Failed to receive message");
				
			// It is not possible send non-transactional to a transactional queue
			Queue nonTrxQueue = new Queue(queuename, Queue.AccessSend, false);
			nonTrxQueue.send(msg);
			result = recvQueue.receive();
			if (result != null)
				throw new Exception("It should not be possible to sent non-trx message to trx-queue");
				
			recvQueue.close();
			sendQueue.close();
			nonTrxQueue.close();
		}
		
		Queue.delete(queuename);
		
		try
		{
			Queue recvQueue = new Queue(queuename, Queue.AccessReceive, true);
			throw new Exception("Failed to delete queue");
		}
		catch(MessageQueueException ex)
		{
		}
	}
	
	//@Test
	public void testBinaryMessages()
		throws Exception
	{
		try
		{
			Queue.delete(queuename);
		}
		catch(MessageQueueException ex)
		{}
		Queue.create(createQueuename, "MsmqJavaTestRunner", true);
		
		{
			Queue recvQueue = new Queue(queuename, Queue.AccessReceive, true); 
			Queue sendQueue = new Queue(queuename, Queue.AccessSend, true);

			String msg = "Hello";
			byte[] utf8 = msg.getBytes("UTF-8");

			Message request = new Message(utf8, utf8.length, Message.VT_BIN, msg, msg);
			sendQueue.send(request);
			Message reply = recvQueue.receive();
			
			if (reply.getBinaryMessage().length!=utf8.length)
				throw new Exception("Failed to receive message");
		
			String msgReply = new String(reply.getBinaryMessage(), "UTF-8");
			if (!msg.equals( msgReply ))
				throw new Exception("Failed to receive message");
				
			if (reply.getMessageType() != Message.VT_BIN)
				throw new Exception("Failed to receive message type");
				
			if (!msg.equals( reply.getCorrelationId() ))
				throw new Exception("Failed to receive CorrelationId");
				
			if (!msg.equals( reply.getLabel() ))
				throw new Exception("Failed to receive Label(");

			// Allocate a large buffer and see that it only sends the specified part
			byte[] bigSendbuffer = new byte[2*1024*1024];
			bigSendbuffer[bigSendbuffer.length / 4] = 1;
			request = new Message(bigSendbuffer, bigSendbuffer.length / 2, Message.VT_BIN, "", "");
			sendQueue.send(request);
			reply = recvQueue.receive();
			
			if (reply.getMessageLength()!=bigSendbuffer.length/2)
				throw new Exception("Failed to send/receive big message");

			if (reply.getBinaryMessage().length!=bigSendbuffer.length/2)
				throw new Exception("Failed to send/receive big message");
				
			if (reply.getBinaryMessage()[bigSendbuffer.length / 4] != 1)
				throw new Exception("Failed to send/receive big message");
				
			// Pre-Allocate receive buffer and see that it is reused for receiving
			byte[] bigRecvBuffer = new byte[3*1024*1024];
			sendQueue.send(request);
			reply = new Message(bigRecvBuffer, 0, Message.VT_EMPTY, null, null);
			reply = recvQueue.receive(reply, 0);
			if (reply.getBinaryMessage()!=bigRecvBuffer)
				throw new Exception("Failed to send/receive big message with buffer reuse");
				
			if (bigRecvBuffer.length != 3*1024*1024)
				throw new Exception("Failed to send/receive big message with buffer reuse");
				
			if (reply.getMessageLength() != bigSendbuffer.length / 2)
				throw new Exception("Failed to send/receive big message with buffer reuse");
				
			recvQueue.close();
			sendQueue.close();
		}
		
		Queue.delete(queuename);
		
		try
		{
			Queue recvQueue = new Queue(queuename, Queue.AccessReceive, true);
			throw new Exception("Failed to delete queue");
		}
		catch(MessageQueueException ex)
		{
		}
	}
}
