
class MsmqQueue
{
	private:
		QUEUEHANDLE		hQueue;

	public:
		HRESULT createQueue( const char  *szQueuePath,
                             const char  *szQueueLabel,
                             bool        isTransactional
                             );

		HRESULT deleteQueue( const char  *szQueuePath
			                 );

		HRESULT openQueue(   const char  *szQueuePath,
                             int	     iOpenmode
                             );

		HRESULT read(		 char    *szMessageBody,
                             int     *iMessageBodySize,
							 int     *iMessageBodyType,
                             char    *szLabel,
                             char    *szCorrelationID,
							 bool    isTransactional,
                             int     iTimeout, 
                             bool    isPeekOnly
                             );

		HRESULT sendMessage( const char  *szMessageBody,
                             int         iMessageBodySize,
							 int         iMessageBodyType,
                             const char  *szLabel,
                             const char  *szCorrelationID,
							 bool        isTransactional
                             );

		HRESULT closeQueue( void );

};
