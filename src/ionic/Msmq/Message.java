package ionic.Msmq;

  public class Message {
	public final static int VT_EMPTY= 0;
	public final static int VT_BSTR	= 8;
	public final static int VT_UI1	= 17;
	public final static int VT_ARRAY= 0x2000;
	public final static int VT_BIN	= VT_ARRAY | VT_UI1;
  
	/** Message is using VT_ARRAY (Accessed directly by JNI) */
	byte _message[];
	/** Message type VT_BSTR / VT_ARRAY | (Accessed directly by JNI) */
	int _messageType;
	/** Message length (Accessed directly by JNI) */
	int _messageLength;
	/** Message label (Accessed directly by JNI) */
    String _label;
	/** Message foreign id (Accessed directly by JNI) */
	String _correlationId;

    public void setMessage(String value)
		throws java.io.UnsupportedEncodingException
	{
		if (value==null)
			setBinaryMessage(null, 0, VT_EMPTY);
		else
		{
			byte[] unicode = value.getBytes("UTF-16LE");	// Will not generate a BOM (Byte Order Mark)
			setBinaryMessage(unicode, unicode.length, VT_BSTR);
		}
	}
    public String getMessage() throws java.io.UnsupportedEncodingException { return new String(_message, "UTF-16LE"); }
	
	public void setBinaryMessage(byte value[], int messageLength, int messageType)
	{
		_message = value;
		_messageType = messageType;
		_messageLength = messageLength;
	}
	public byte[] getBinaryMessage() { return _message; }
	
	public int getMessageType() { return _messageType; }
	
	public int getMessageLength() { return _messageLength; }

    public void setLabel(String value) { _label= value; }
    public String getLabel() { return _label; }

    public void setCorrelationId(String value) { _correlationId = value; }
    public String getCorrelationId() { return _correlationId; }
	
	public  Message()
	{
		this(null, 0, VT_EMPTY, "", "");
	}

    public  Message(String message, String label, String correlationId) 
		throws java.io.UnsupportedEncodingException
	{
      setMessage(message);
      _label= label;
      _correlationId= correlationId;
    }
	
    public  Message(byte message[], int messageLength, int messageType, String label, String correlationId)
	{
	  setBinaryMessage(message, messageLength, messageType);
      _label= label;
      _correlationId= correlationId;
    }
  }
