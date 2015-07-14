// MsmqQueue.cpp 
//
// See the MQ Functions Ref: 
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/msmq/msmq_ref_functions_4o37.asp

#include <stdio.h>
#include <MqOai.h>
#include <mq.h>

#include "MsmqQueue.hpp"


HRESULT MsmqQueue::createQueue( const char *szQueuePath, 
				  const char *szQueueLabel,
				  bool isTransactional )
{
    HRESULT hr = MQ_OK;
    int len;

    // example: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/msmq/msmq_using_createqueue_0f51.asp
    const int NUMBEROFPROPERTIES = 5;

    MQQUEUEPROPS   QueueProps;
    MQPROPVARIANT  aQueuePropVar[NUMBEROFPROPERTIES];
    QUEUEPROPID    aQueuePropId[NUMBEROFPROPERTIES];
    HRESULT        aQueueStatus[NUMBEROFPROPERTIES];
    DWORD          i = 0;
  
    if (szQueuePath == NULL) 
	    return MQ_ERROR_INVALID_PARAMETER;

    WCHAR wszPathName[MQ_MAX_Q_NAME_LEN] = {0};
    len= strlen(szQueuePath);
    if (MultiByteToWideChar(
			    CP_ACP,
			    0,
			    szQueuePath,
			    len,
			    wszPathName,
			    _countof( wszPathName ) ) == 0)
	{
	    return MQ_ERROR_INVALID_PARAMETER;
	}

    if (len < _countof( wszPathName ) )
		wszPathName[len]= 0; // need this to terminate

    WCHAR wszLabel[MQ_MAX_Q_LABEL_LEN] = {0};
    len= strlen(szQueueLabel);
    if (MultiByteToWideChar(
			    CP_ACP,
			    0,
			    szQueueLabel,
			    len,
			    wszLabel,
			    _countof( wszLabel ) ) == 0)
	{
	    return MQ_ERROR_INVALID_PARAMETER;
	}
    if (len < _countof( wszLabel ) )
		wszLabel[len]= 0; // need this to terminate


    printf("attempting to create queue with name= '%S', label='%S'\n", wszPathName, wszLabel);

    // Set the PROPID_Q_PATHNAME property with the path name provided.
    aQueuePropId[i] = PROPID_Q_PATHNAME;
    aQueuePropVar[i].vt = VT_LPWSTR;
    aQueuePropVar[i].pwszVal = wszPathName; // wszActualName
    i++;

    // Set optional queue properties. PROPID_Q_TRANSACTIONAL
    // must be set to make the queue transactional.
    aQueuePropId[i] = PROPID_Q_TRANSACTION;
    aQueuePropVar[i].vt = VT_UI1;
    aQueuePropVar[i].bVal = (unsigned char) isTransactional;
    i++;

	// 
    aQueuePropId[i] = PROPID_Q_LABEL;
    aQueuePropVar[i].vt = VT_LPWSTR;
    aQueuePropVar[i].pwszVal = wszLabel;
    i++;

    // Initialize the MQQUEUEPROPS structure 
    QueueProps.cProp = i;                  //Number of properties
    QueueProps.aPropID = aQueuePropId;     //IDs of the queue properties
    QueueProps.aPropVar = aQueuePropVar;   //Values of the queue properties
    QueueProps.aStatus = aQueueStatus;     //Pointer to return status

    // http://msdn.microsoft.com/library/en-us/msmq/msmq_ref_functions_8dut.asp
	WCHAR wszFormatName[MQ_MAX_Q_NAME_LEN] = {0};
	DWORD dwFormatNameLength = _countof(wszFormatName);
    hr = MQCreateQueue(NULL,                // Security descriptor
		       &QueueProps,         // Address of queue property structure
				wszFormatName,       // Pointer to format name buffer
		       &dwFormatNameLength);  // Pointer to receive the queue's format name length
    return hr;

};



HRESULT MsmqQueue::deleteQueue( const char *szQueuePath )
{
    HRESULT hr = MQ_OK;

    // example: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/msmq/msmq_using_createqueue_0f51.asp
    if (szQueuePath == NULL) 
	    return MQ_ERROR_INVALID_PARAMETER;

	int len = strlen(szQueuePath);
    WCHAR wszPathName[MQ_MAX_Q_NAME_LEN] = {0};
    if (MultiByteToWideChar(
			    CP_ACP,
			    0,
			    szQueuePath,
			    len,
			    wszPathName,
			    _countof( wszPathName ) ) == 0)
	{
	    return MQ_ERROR_INVALID_PARAMETER;
	}

    if (len < _countof( wszPathName ) )
		wszPathName[len]= 0; // need this to terminate
		
	printf("attempting to delete queue with name= '%S'\n", wszPathName);

    hr = MQDeleteQueue( wszPathName );

    return hr;
};



HRESULT MsmqQueue::openQueue(const char *szQueuePath, int openmode)
{
    WCHAR wszPathName[MQ_MAX_Q_NAME_LEN] = {0};
    long	accessmode = openmode;  // bit field: MQ_{RECEIVE,SEND,PEEK,ADMIN}_ACCESS, 
    long	sharemode = MQ_DENY_NONE;

    // Validate the input string.
    if (szQueuePath == NULL)
	{
	    return MQ_ERROR_INVALID_PARAMETER;
	}

    // convert to wide characters;
	int len = strlen(szQueuePath);
    if (MultiByteToWideChar(
			    CP_ACP,
			    0,
			    szQueuePath,
			    len,
			    wszPathName,
			    _countof(wszPathName) ) == 0)
	{
	    return MQ_ERROR_INVALID_PARAMETER;
	}
    if (len < _countof( wszPathName ) )
		wszPathName[len]= 0; // need this to terminate	
	

    HRESULT hr = MQ_OK;

    // dinoch Mon, 18 Apr 2005  16:12

      printf("open: ");
      printf("fmtname(%ls) ", wszPathName);
      printf("accessmode(%d) ", accessmode);
      printf("sharemode(%d) ", sharemode);
     printf("\n");

    hr = MQOpenQueue(
		     wszPathName,           // Format name of the queue
		     accessmode,		  // Access mode
		     sharemode,  	          // Share mode
		     &hQueue   	                  // OUT: Handle to queue
		     );
			 
    // Retry to handle AD replication delays. 
    //
    if (hr == MQ_ERROR_QUEUE_NOT_FOUND)
	{
	    int iCount = 0 ;
	    while((hr == MQ_ERROR_QUEUE_NOT_FOUND) && (iCount < 120))
		{
		    printf(".");

		    // Wait a bit.
		    iCount++ ;
		    Sleep(50);

		    // Retry.
		    hr = MQOpenQueue(wszPathName, 
				     accessmode,
				     sharemode,
				     &hQueue);
		}
	}

    if (FAILED(hr))
	{
	    MQCloseQueue(hQueue);
	}

    return hr;

};



HRESULT MsmqQueue::read(     char    *szMessageBody,
                             int     *iMessageBodySize,
							 int     *iMessageBodyType,
                             char    *szLabel,
                             char    *szCorrelationID,
							 bool    isTransactional,
                             int     iTimeout, 
                             bool    isPeekOnly
                             )
{
    const int NUMBEROFPROPERTIES = 8;
    DWORD i = 0;
    HRESULT hr = MQ_OK;
	
    DWORD dwAction= isPeekOnly ? MQ_ACTION_PEEK_CURRENT : MQ_ACTION_RECEIVE;
	ITransaction* pTransaction = isTransactional ? MQ_SINGLE_MESSAGE : MQ_NO_TRANSACTION;

    MQMSGPROPS msgprops;
    MSGPROPID aMsgPropId[NUMBEROFPROPERTIES];
    PROPVARIANT aMsgPropVar[NUMBEROFPROPERTIES];

	// Message Body Buffer
	if (szMessageBody!=NULL)
	{
		memset(szMessageBody, 0, *iMessageBodySize);
		aMsgPropId[i] = PROPID_M_BODY;
		aMsgPropVar[i].vt = VT_VECTOR | VT_UI1;
		aMsgPropVar[i].caub.pElems = (LPBYTE)szMessageBody;
		aMsgPropVar[i].caub.cElems = *iMessageBodySize;
		i++;
	}

	// Message Body Size
	aMsgPropId[i] = PROPID_M_BODY_SIZE;
	aMsgPropVar[i].vt = VT_NULL;
	i++;
	
	// Message Body Type
	aMsgPropId[i] = PROPID_M_BODY_TYPE;
	aMsgPropVar[i].vt = VT_NULL;
	i++;

	// Correlation ID
	aMsgPropId[i] = PROPID_M_CORRELATIONID;
    aMsgPropVar[i].vt = VT_VECTOR | VT_UI1;
    aMsgPropVar[i].caub.pElems = (LPBYTE)szCorrelationID;
    aMsgPropVar[i].caub.cElems = PROPID_M_CORRELATIONID_SIZE;
	memset(szCorrelationID, 0, PROPID_M_CORRELATIONID_SIZE);
    i++;

	// Label Size
    aMsgPropId[i] = PROPID_M_LABEL_LEN;            // Property ID
    aMsgPropVar[i].vt =VT_UI4;                     // Type indicator
    aMsgPropVar[i].ulVal = MQ_MAX_MSG_LABEL_LEN;   // Label buffer size
    i++;

	// Label Buffer
    WCHAR wszLabelBuffer[MQ_MAX_MSG_LABEL_LEN] = {0};
    aMsgPropId[i] = PROPID_M_LABEL;
    aMsgPropVar[i].vt = VT_LPWSTR;
    aMsgPropVar[i].pwszVal = wszLabelBuffer;
    i++;

    // NB: if you want to get PROPID_M_RESP_QUEUE, you must also request PROPID_M_RESP_QUEUE_LEN. 
    // that implies 2 additional properties to request. 
    // see http://msdn.microsoft.com/library/en-us/msmq/html/ea8cfb28-43e0-4041-9add-11080375690c.asp

    msgprops.cProp = i;                         // Number of message properties
    msgprops.aPropID = aMsgPropId;                    // IDs of the message properties
    msgprops.aPropVar = aMsgPropVar;                  // Values of the message properties
    msgprops.aStatus  = 0;			  // Error reports

    hr = MQReceiveMessage(
			  hQueue,             // Handle to the destination queue
			  iTimeout,	      // Time out interval
			  dwAction,           // Peek?  or Dequeue.  Receive action
			  &msgprops,          // Pointer to the MQMSGPROPS structure
			  NULL, NULL, NULL,   // No OVERLAPPED structure etc.
			  pTransaction  // MQ_SINGLE_MESSAGE | MQ_MTS_TRANSACTION |
			                     // MQ_XA_TRANSACTION   
			  );
			  
	// http://msdn.microsoft.com/library/en-us/intl/unicode_2bj9.asp
	memset(szLabel, 0, MQ_MAX_MSG_LABEL_LEN);
	if (0 != WideCharToMultiByte(
					 CP_ACP,                // code page
					 0,                    // conversion flags
					 wszLabelBuffer,     // wide-character string to convert
					 (int) wcslen(wszLabelBuffer), // number of chars in string.
					 szLabel,                // buffer for new string
					 MQ_MAX_MSG_LABEL_LEN,         // size of buffer
					 NULL,                // (LPCSTR) default for unmappable chars
					 NULL                 // (LPBOOL) set when default char used
					 )) {
		// actually converted, so... we are happy...
	}

	for(ULONG prop = 0 ; prop < msgprops.cProp; ++prop)
	{
		if (msgprops.aPropID[prop]==PROPID_M_BODY_SIZE)
			*iMessageBodySize = msgprops.aPropVar[prop].ulVal;
		if (msgprops.aPropID[prop]==PROPID_M_BODY_TYPE)
			*iMessageBodyType = msgprops.aPropVar[prop].ulVal;
	}


    return hr;
};




HRESULT MsmqQueue::sendMessage(   const char  *szMessageBody,
                                  int         iMessageBodySize,
							      int         iMessageBodyType,
                                  const char  *szLabel,
                                  const char  *szCorrelationID,
							      bool        isTransactional
                                  )
{
    const int NUMBEROFPROPERTIES = 8;                 // Number of properties
    DWORD i = 0;
    HRESULT hr = MQ_OK;
    CHAR  aszCorrelationID[PROPID_M_CORRELATIONID_SIZE+1] = {0};
	
    // transactionFlag:  MQ_NO_TRANSACTION, MQ_MTS_TRANSACTION, MQ_XA_TRANSACTION, or MQ_SINGLE_MESSAGE 
    // see mq.h for details...
	ITransaction* pTransaction = isTransactional ? MQ_SINGLE_MESSAGE : MQ_NO_TRANSACTION;

    int len = strlen(szCorrelationID); 
    if (len > 0) 
	{
		if (len > PROPID_M_CORRELATIONID_SIZE) 
			len = PROPID_M_CORRELATIONID_SIZE;
		// copy across JMS correlationID truncating to MSMQ msgid size of 20
		memcpy(aszCorrelationID, szCorrelationID, len);
    }

    // Define an MQMSGPROPS structure.
    MQMSGPROPS msgprops;
    MSGPROPID aMsgPropId[NUMBEROFPROPERTIES];
    PROPVARIANT aMsgPropVar[NUMBEROFPROPERTIES];
    HRESULT aMsgStatus[NUMBEROFPROPERTIES];
	
	//Set the body
    aMsgPropId[i] = PROPID_M_BODY;
    aMsgPropVar[i].vt = VT_VECTOR | VT_UI1;
    aMsgPropVar[i].caub.pElems = (LPBYTE)szMessageBody; //szFormattedMessageBody
    aMsgPropVar[i].caub.cElems = iMessageBodySize ;
    ++i;
	
	// Message Body Type
	aMsgPropId[i] = PROPID_M_BODY_TYPE;
	aMsgPropVar[i].vt = VT_UI4;
	aMsgPropVar[i].ulVal = iMessageBodyType;	// VT_BSTR / VT_ARRAY | VT_UI1
	i++;
	
	// Specify that the message should not be lost if restarting the computer
    aMsgPropId[i] = PROPID_M_DELIVERY;
    aMsgPropVar[i].vt = VT_UI1;
    aMsgPropVar[i].bVal = MQMSG_DELIVERY_RECOVERABLE;
    i++;

	//Set the correlation id
    aMsgPropId[i] = PROPID_M_CORRELATIONID;
    aMsgPropVar[i].vt = VT_VECTOR | VT_UI1;
    aMsgPropVar[i].caub.pElems = (LPBYTE)aszCorrelationID;
    aMsgPropVar[i].caub.cElems = PROPID_M_CORRELATIONID_SIZE;
    ++i;

	//Set the label
    aMsgPropId[i] = PROPID_M_LABEL;
    aMsgPropVar[i].vt = VT_LPWSTR;

    WCHAR wszLabel[MQ_MAX_MSG_LABEL_LEN] = {0};
    len= strlen(szLabel);
    if (len > 0) {
	    if (MultiByteToWideChar(
				    (UINT) CP_ACP,
				    (DWORD) 0,
				    (LPCSTR) szLabel,
				    len,
				    wszLabel,
				    _countof(wszLabel) ) == 0)
		{
		    return MQ_ERROR_INVALID_PARAMETER;
		}
	    if (len < _countof(wszLabel) )
			wszLabel[len]=0; // terminate
	}
    aMsgPropVar[i].pwszVal = wszLabel;
    ++i;

    // Initialize the MQMSGPROPS structure.
    msgprops.cProp = i;              // Number of message properties
    msgprops.aPropID = aMsgPropId;   // IDs of the message properties
    msgprops.aPropVar = aMsgPropVar; // Values of the message properties
    msgprops.aStatus  = aMsgStatus;  // Error reports
	
    // Call MQSendMessage to put the message to the queue. 
    hr = MQSendMessage(
		       hQueue,             // Handle to the destination queue
		       &msgprops,          // Pointer to the MQMSGPROPS structure
		       pTransaction  
		       );                  

    return hr;
};


HRESULT MsmqQueue::closeQueue( )
{
    HRESULT hr = MQ_OK;
    hr = MQCloseQueue(hQueue);
    return hr;
};


