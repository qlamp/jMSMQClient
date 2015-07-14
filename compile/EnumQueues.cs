
using System;
using System.Messaging;

public class TestDriver
{
  static void Main(String[] args)
  {
    string MachineName= ".";
    // get the list of message queues
    MessageQueue[] MQList = MessageQueue.GetPrivateQueuesByMachine(MachineName);

    // check to make sure we found some private queues on that machine
    if (MQList.Length >0)
    {
   
   string[,] MQNameList = new string[MQList.Length, 4];

      // loop through all message queues and get the name, path, etc.
      for (int i = 0; i < MQList.Length; i++)
      {
	try {
	  MQNameList[i,0]= MQList[i].QueueName;
	}
	catch {
	  MQNameList[i,0]= "???";
	}
	try {
	  MQNameList[i,1]= MQList[i].Label;
	}
	catch {
	  MQNameList[i,1]= "???";
	}
	try {
	  MQNameList[i,2]= MQList[i].Transactional.ToString();
	}
	catch {
	  MQNameList[i,2]= "???";
	}
	try {
	  MQNameList[i,3]= MQList[i].FormatName;
	}
	catch {
	  MQNameList[i,3]= "???";
	}


	System.Console.WriteLine("{0} '{1}'  {2}  {3}", 
				 MQNameList[i,0],
				 MQNameList[i,1],
				 MQNameList[i,2],
				 MQNameList[i,3]);
      }
    } 

  }
}


