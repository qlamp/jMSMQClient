// MsmqQueueNativeMethods.cpp 
//
// Native methods for MSMQ APIs, to be exposed into the Java Queue class via JNI.
//

#include <stdio.h>
#include <WTypes.h>   // reqd for WinBase.h
#include <WinBase.h>  // for CriticalSection
#include <MqOai.h>
#include <mq.h>

#include "JniMsmq.h"
#include "MsmqQueue.hpp"

#pragma warning (disable: 4100)		// unreferenced formal parameter

void SetJavaString (JNIEnv * jniEnv, jobject object, char * fieldName, const char * valueToSet) 
{
    jclass cls = jniEnv->GetObjectClass(object);
    jfieldID fieldId = jniEnv->GetFieldID(cls, fieldName, "Ljava/lang/String;");
    if (fieldId !=0)
	{
		// Allocate the string and initialize with value
	    jstring value = jniEnv->NewStringUTF(valueToSet);
		// Update the member variable
		jniEnv->SetObjectField(object, fieldId, value);
	}
}

void SetJavaByteArray (JNIEnv * jniEnv, jobject object, char * fieldName, const char * valueToSet, int valueLength)
{
	jclass cls = jniEnv->GetObjectClass(object);
	jfieldID fieldId =  jniEnv->GetFieldID(cls, fieldName, "[B");	// Array of bytes
    if (fieldId !=0)
	{
		// Allocate new  buffer
		jbyteArray nativeData = jniEnv->NewByteArray(valueLength);
		// Initialize the buffer with value
		char* nativeBuffer = (char*)jniEnv->GetByteArrayElements(nativeData, NULL);
		memcpy(nativeBuffer, valueToSet, valueLength);
		// Commit the changes to the buffer
		jniEnv->ReleaseByteArrayElements(nativeData, (jbyte *)nativeBuffer, 0);
		// Update the member variable
		jniEnv->SetObjectField(object, fieldId, nativeData);
	}
}

jbyteArray GetJavaByteArray (JNIEnv * jniEnv, jobject object, char * fieldName)
{
	jclass cls = jniEnv->GetObjectClass(object);
	jfieldID fieldId = jniEnv->GetFieldID(cls, fieldName, "[B");	// Array of bytes
    if (fieldId !=0)
	{
		jbyteArray nativeData = (jbyteArray) jniEnv->GetObjectField(object, fieldId);
		return nativeData;
	}
	return NULL;
}

jstring GetJavaString (JNIEnv * jniEnv, jobject object, char * fieldName)
{
	jclass cls = jniEnv->GetObjectClass(object);
	jfieldID fieldId = jniEnv->GetFieldID(cls, fieldName, "Ljava/lang/String;");
    if (fieldId !=0)
	{
		jstring nativeData = (jstring) jniEnv->GetObjectField(object, fieldId);
		return nativeData;
	}
	return NULL;
}

void SetJavaInt (JNIEnv * jniEnv, jobject object, char * fieldName, int valueToSet)
{
	jclass cls = jniEnv->GetObjectClass(object);
	jfieldID fieldId = jniEnv->GetFieldID(cls, fieldName, "I");	// Integer
    if (fieldId !=0)
	{
		jniEnv->SetIntField(object,fieldId,valueToSet);
	}
}

jint GetJavaInt (JNIEnv * jniEnv, jobject object, char * fieldName)
{
	jclass cls = jniEnv->GetObjectClass(object);
	jfieldID fieldId = jniEnv->GetFieldID(cls, fieldName, "I");	// Integer
    if (fieldId !=0)
	{
		jint nativeData = jniEnv->GetIntField(object,fieldId);
		return nativeData;
	}
	return 0;
}

// static // 
JNIEXPORT jint JNICALL Java_ionic_Msmq_Queue_nativeCreateQueue
 	(JNIEnv *jniEnv, jclass clazz, jstring queuePath, jstring queueLabel, jboolean isTransactional)
{
    HRESULT hr = 0;
    try {
		const char * szQueuePath = jniEnv->GetStringUTFChars(queuePath, 0);
		const char * szQueueLabel = jniEnv->GetStringUTFChars(queueLabel, 0);

		hr = MsmqQueue().createQueue(szQueuePath, 
									szQueueLabel, 
									isTransactional ? true : false);

		jniEnv->ReleaseStringUTFChars(queuePath, szQueuePath);
		jniEnv->ReleaseStringUTFChars(queueLabel, szQueueLabel);
    }
    catch(...) {
		printf("createQueue caught an error..\n");
		jniEnv->ExceptionDescribe();
		jniEnv->ExceptionClear();
		hr = -99;
    }

    fflush(stdout);
    return (jint) hr;
}


// static // 
JNIEXPORT jint JNICALL Java_ionic_Msmq_Queue_nativeDeleteQueue
 	(JNIEnv *jniEnv, jclass clazz, jstring queuePath)
{
    HRESULT hr = 0;
    try {
		const char * szQueuePath = jniEnv->GetStringUTFChars(queuePath, 0);

		hr = MsmqQueue().deleteQueue((char *) szQueuePath) ; 

		jniEnv->ReleaseStringUTFChars(queuePath, szQueuePath);
    }
    catch(...) {
		printf("deleteQueue caught an error..\n");
		jniEnv->ExceptionDescribe();
		jniEnv->ExceptionClear();
		hr = -99;
    }

    fflush(stdout);
    return (jint) hr;
}

JNIEXPORT jint JNICALL Java_ionic_Msmq_Queue_nativeOpenQueueWithAccess
	(JNIEnv *jniEnv, jobject object, jlongArray queueHandles, jstring queuePath, jint access)
{
    HRESULT hr = -99;

    try {
		if (jniEnv->GetArrayLength(queueHandles)<2)
			return hr;
	
		const char *szQueuePath = jniEnv->GetStringUTFChars(queuePath, 0);
		printf("OpenQueueWithAccess (%s)\n", szQueuePath ); 

		jlong* nativeQueueHandles = jniEnv->GetLongArrayElements(queueHandles, NULL);

		MsmqQueue* receiveQueue = NULL;
		MsmqQueue* sendQueue = NULL;
		if (access & MQ_RECEIVE_ACCESS || access & MQ_PEEK_ACCESS)
		{
			int receiveAccess = access & MQ_RECEIVE_ACCESS ? MQ_RECEIVE_ACCESS : MQ_PEEK_ACCESS;
			if (access & MQ_ADMIN_ACCESS)
				receiveAccess |= MQ_ADMIN_ACCESS;
			receiveQueue = new MsmqQueue;
			hr = receiveQueue->openQueue((char *) szQueuePath, receiveAccess );
			if (hr != 0) {
				delete receiveQueue;
				jniEnv->ReleaseStringUTFChars(queuePath, szQueuePath);
				jniEnv->ReleaseLongArrayElements(queueHandles, nativeQueueHandles, JNI_ABORT);
				return hr;
			}
			nativeQueueHandles[0] = (jlong)receiveQueue;
		}
		if (access & MQ_SEND_ACCESS)
		{
			int sendAccess = MQ_SEND_ACCESS;
			if (access & MQ_ADMIN_ACCESS)
				sendAccess |= MQ_ADMIN_ACCESS;
			sendQueue = new MsmqQueue;
			hr = sendQueue->openQueue((char *) szQueuePath, sendAccess);
			if (hr != 0) {
				delete sendQueue;
				if (receiveQueue!=NULL)
				{
					receiveQueue->closeQueue();
					delete receiveQueue;
				}
				jniEnv->ReleaseStringUTFChars(queuePath, szQueuePath);
				jniEnv->ReleaseLongArrayElements(queueHandles, nativeQueueHandles, JNI_ABORT);
				return hr;
			}
			nativeQueueHandles[1] = (jlong)sendQueue;
		}
		
		jniEnv->ReleaseStringUTFChars(queuePath, szQueuePath);
		jniEnv->ReleaseLongArrayElements(queueHandles, nativeQueueHandles, 0);
    }
    catch(...) {
		printf("openQueue : Exception. \n");
			jniEnv->ExceptionDescribe();
			jniEnv->ExceptionClear();
		hr = -99;
    }
	
    fflush(stdout);

    return (jint) hr;
}

JNIEXPORT jint JNICALL Java_ionic_Msmq_Queue_nativeReceive
 	(JNIEnv *jniEnv, jobject object, jlongArray queueHandles, jobject message, jboolean isTransactional, jint timeout, jboolean peek)
{
    HRESULT  hr = -99;
	
	char* szMessageBody = NULL;
	int iMessageBodyLength = 0;
	int iMessageBodyType = 0;
	bool  bMessageBodyAllocated = false;
	
	jbyteArray nativeData = 0;
	jsize nativeDataSize = 0;

    try 
	{
		if (jniEnv->GetArrayLength(queueHandles)<2)
			return hr;

		jlong* nativeQueueHandles = jniEnv->GetLongArrayElements(queueHandles, NULL);
		MsmqQueue *q = (MsmqQueue*)nativeQueueHandles[0];
		jniEnv->ReleaseLongArrayElements(queueHandles, nativeQueueHandles, JNI_ABORT);
		if (q==NULL)
			return hr;

		char szCorrelationID[PROPID_M_CORRELATIONID_SIZE];
		char szLabel[MQ_MAX_MSG_LABEL_LEN];
		
		nativeData = GetJavaByteArray(jniEnv, message, "_message");
		nativeDataSize = nativeData != NULL ? jniEnv->GetArrayLength(nativeData) : 0;
		if (nativeDataSize > 0)
		{
			szMessageBody = (char*)jniEnv->GetByteArrayElements(nativeData, NULL);
			iMessageBodyLength = nativeDataSize;
		}
		
		hr = q->read(szMessageBody,
					 &iMessageBodyLength,
					 &iMessageBodyType,
					 szLabel, 
					 szCorrelationID, 
					 isTransactional ? true : false,
					 timeout, 
					 peek || szMessageBody==NULL);
					 
		if (hr == 0 && szMessageBody==NULL && iMessageBodyLength > 0)
		{
			// We have now have the buffer size, and can now do a proper read
			szMessageBody = (char*)malloc(iMessageBodyLength);
			bMessageBodyAllocated = true;
			hr = q->read(szMessageBody,
				 &iMessageBodyLength,
				 &iMessageBodyType,
				 szLabel, 
				 szCorrelationID, 
				 isTransactional ? true : false,
				 timeout, 
				 peek ? true : false);
		}
		
		if (hr==0) 
		{
			if (bMessageBodyAllocated)
				SetJavaByteArray(jniEnv, message, "_message", szMessageBody, iMessageBodyLength); 
			else
			{
				nativeDataSize = 0;	// Mark that we have released the buffer
				jniEnv->ReleaseByteArrayElements(nativeData, (jbyte *)szMessageBody, 0);
			}
			
			SetJavaInt(jniEnv, message, "_messageLength", iMessageBodyLength);
			SetJavaInt(jniEnv, message, "_messageType", iMessageBodyType);
			SetJavaString(jniEnv, message, "_label", szLabel);
			SetJavaString(jniEnv, message, "_correlationId", szCorrelationID); 
		}
    }
    catch(...) 
	{
		printf("Read() : Exception\n");
		jniEnv->ExceptionDescribe();
		jniEnv->ExceptionClear();
		hr = -99;
    }
	
	if (nativeDataSize > 0)
	{
		jniEnv->ReleaseByteArrayElements(nativeData, (jbyte *)szMessageBody, JNI_ABORT);
	}
	
	if (bMessageBodyAllocated)
	{
		free(szMessageBody);
		szMessageBody = NULL;
	}

    fflush(stdout);
    return (jint) hr;
}

JNIEXPORT jint JNICALL Java_ionic_Msmq_Queue_nativeSend
 	(JNIEnv *jniEnv, 
	 jobject object,
	 jlongArray queueHandles,
	 jobject message,
	 jboolean isTransactional
	 )
{
	HRESULT hr = -99;
	
	char* szMessageBody = NULL;
	int iMessageBodyLength = 0;
	int iMessageBodyType = 0;
	
	jbyteArray nativeData = NULL;
	jsize nativeDataSize = NULL;
	
	jstring nativeLabel = NULL;
	jstring nativeCorrelationId = NULL;
	
	const char *szLabel = NULL;
	const char *szCorrelationID = NULL;

    try 
	{
		if (jniEnv->GetArrayLength(queueHandles)<2)
			return hr;	
	
		jlong* nativeQueueHandles = jniEnv->GetLongArrayElements(queueHandles, NULL);
		MsmqQueue *q = (MsmqQueue*)nativeQueueHandles[1];
		jniEnv->ReleaseLongArrayElements(queueHandles, nativeQueueHandles, JNI_ABORT);
		if (q==NULL)
			return hr;
		
		iMessageBodyType = GetJavaInt(jniEnv, message, "_messageType");
		iMessageBodyLength = GetJavaInt(jniEnv, message, "_messageLength");
		
		nativeData = GetJavaByteArray(jniEnv, message, "_message");
		nativeDataSize = nativeData != NULL ? jniEnv->GetArrayLength(nativeData) : 0;
		szMessageBody = (char*)jniEnv->GetByteArrayElements(nativeData, NULL);;
		
		nativeLabel = GetJavaString(jniEnv, message, "_label");
		szLabel = jniEnv->GetStringUTFChars(nativeLabel, 0);
		
		nativeCorrelationId = GetJavaString(jniEnv, message, "_correlationId");
		szCorrelationID = jniEnv->GetStringUTFChars(nativeCorrelationId, 0);
		
		hr = q->sendMessage( szMessageBody, 
					iMessageBodyLength,
					iMessageBodyType,
					szLabel, 
					szCorrelationID, 
					isTransactional ? true : false
					);
    }
    catch(...) {
		jniEnv->ExceptionDescribe();
		jniEnv->ExceptionClear();
		hr = -99;
    }
	
	if (szMessageBody != NULL)
		jniEnv->ReleaseByteArrayElements(nativeData, (jbyte*)szMessageBody, JNI_ABORT);
	if (szLabel != NULL)
		jniEnv->ReleaseStringUTFChars(nativeLabel, szLabel);
	if (szCorrelationID != NULL)
		jniEnv->ReleaseStringUTFChars(nativeCorrelationId, szCorrelationID);
		
    fflush(stdout);
    return (jint) hr;
}

JNIEXPORT jint JNICALL Java_ionic_Msmq_Queue_nativeClose
 		(JNIEnv *jniEnv, jobject object, jlongArray queueHandles)
{
    HRESULT hr = -99;
	
    try {
		if (jniEnv->GetArrayLength(queueHandles)<2)
			return hr;
	
		jlong* nativeQueueHandles = jniEnv->GetLongArrayElements(queueHandles, NULL);
		MsmqQueue *receiveQueue = (MsmqQueue*)nativeQueueHandles[0];
		MsmqQueue *sendQueue = (MsmqQueue*)nativeQueueHandles[1];
		jniEnv->ReleaseLongArrayElements(queueHandles, nativeQueueHandles, JNI_ABORT);
		
		if (receiveQueue!=NULL)
		{
			hr = receiveQueue->closeQueue();
			delete receiveQueue;
		}
		if (sendQueue!=NULL)
		{
			hr = sendQueue->closeQueue();
			delete sendQueue;
		}
    }
    catch(...) {
		jniEnv->ExceptionDescribe();
		jniEnv->ExceptionClear();
		hr = -99;
    }

    fflush(stdout);
    return (jint) hr;
}

