# jMSMQClient
This is a MSMQ client in Java supporting transactional and non-transactional types, you can run it under 64bit JRE, also you can compile the native part into 32bit to adopt to 32bit JRE. Any questions please contact me, I will try my best to provide help.

Please be advised that it's someone else initializing the code base than me, I found it and made additional changes, otherwise please let me know if you are the original author.

Base Knowledge on Private Queues

Private queues are queues that are not published in Active Directory and are displayed only on the local computer that contains them. Private queues have the following features:

Message Queuing registers private queues locally by storing a description of the queue in the LQS (local queue storage) directory on the local computer. In MSMQ 2.0, and Message Queuing 3.0, the default location is %windir%\system32\msmq\storage\lqs. Note that a description of each public queue created on the local computer is also stored locally in a separate file in the LQS folder.

Private queues are registered on the local computer, not in the directory service, and typically cannot be located by other Message Queuing applications.

Private queues are accessible only by Message Queuing applications that know the full path name, the direct format name, or the private format name of the queue, as follows:

Path name:ComputerName\private$\QueueName.

Path name on local computer: \private$\QueueName.

Direct format name:: DIRECT=ComputerAddress\PRIVATE$\PrivateQueueName.

Private format name: PRIVATE=ComputerGUID\QueueNumber.

For more information on path names and format names, see Queue names.

Private queues have the advantage of no directory service overhead, making them quicker to create, no latency in accessing them, and no replication overhead.

Private queues are not dependent on the directory service, and thus can be created and deleted when the directory service is not working. This is useful for offline operation.

One way that private queues can be exposed to other applications is by setting a message property. To distribute the location of a private queue, an application can send a format name of the private queue as the response queue property of a message.
