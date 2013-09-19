//////////////////////////////////////////////////////////////////////////////////
// CommChannel.cpp - Represents Functionality for File and message transmission	// 
//													 Chunking and Reconstruction//
//				   between peers(in binary as well as text)			            //
// ver 1.0																		//
//   It contains Listner Socket and a Sender Socket with						//
//               (Dynamically Allocated Receiver Sockets)						//
//																				//
//The Threads (i.e. Sender, reciver threads										//
//      use the Communication channel BQ in order to Send/Receive messages)     //	
//																				//
// Language:    Visual C++,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////

#include "ICommChannel.h"
#include "BlockingQueue.h"
#include "Threads.h"
#include "locks.h"
#include <string>
#include <iostream>
# include <sstream>

using namespace CommChannel;

/////////////////////////////////////////////////////////////////////////
  
std::string ToString(int i)
{
  std::ostringstream conv;
  conv << i;
  return conv.str();
}

/////////////////////////////////////////////////////////////////////////
// Helper message converters

typedef std::string stdMessage;

stdMessage ConvertMsgDown(Message^ msg)
{
  stdMessage smsg;
  for(int i=0; i<msg->Length; ++i)
    smsg += (int)msg[i];
  return smsg;
}

Message^ ConvertMsgUp(const stdMessage& smsg)
{
  Message^ msg;
  for(size_t i=0; i<smsg.size(); ++i)
    msg += (wchar_t)smsg[i];
  return msg;
}

typedef BlockingQueue<stdMessage> BQueue;


/////////////////////////////////////////////////////////////////////////
// SendingThread, used to make sure that will work with C++\CLI code

class SendingThread : public tthreadBase  //take data from IPQ and send it to the server
{
public:
	SendingThread(BQueue* pInQ,Socket* s,std::string IP,int pNo) :	pInQ_(pInQ),send(s),ip(IP),portNo(pNo)
	{
	}

  void run()
  {
    send->connect(ip,portNo);

    while(true)
    {
      stdMessage msg = pInQ_->deQ();

       	  sout<<"Connected to Server\n";

		  send->writeLine(msg);

		  sout<<"Disconnected From Server\n";
      
 	  if(msg == "ChannelShutDown")
          break;
    }
	send->disconnect();
  }

  private:
  BQueue* pInQ_;

  Socket * send;
  std::string ip;
  int portNo;
};

/////////////////////////////////////////////////////////////////////////
// RcvrThread, used to make sure that will work with C++\CLI code

class RcvrThread : public tthreadBase//keep listening at the port no and when you get a message, jsut put it in the poutQ
{
public:
	RcvrThread(BQueue* pOutQ,int listenpNo) :pOutQ_(pOutQ) ,listenPortNo(listenpNo)
	{
	}

  void run()
  {
	  stdMessage x;

	  try
	  {
		    SocketListener *listner=new SocketListener(listenPortNo);

			SOCKET s = listner->waitForConnect();
   	
			Socket ip(s);
			
			x=ip.readLine();

			ip.disconnect();
			delete listner;
	  }

	  catch(std::exception e)
	  {
		//  pOutQ_->enQ("Unable to establish connection to server at Port #"+ToString(listenPortNo)+"\nRestart Application");
	  }

	if(x!="")
	   pOutQ_->enQ(x);
  
 }


private:
  BQueue* pOutQ_;
  int listenPortNo;
};



///////////////////////////
//Communication Channel
///////////////////////////

ref class CommunicationChannel : public ICommChannel
{

public:
  CommunicationChannel() : pInQ(new BQueue), pOutQ(new BQueue) 
  {
	  sender=new Socket();
  }

  ~CommunicationChannel()
  {
    delete pInQ;
    delete pOutQ;
  }

  virtual void startSndr() override
  {
	  sThread = new SendingThread(pInQ,sender,ConvertMsgDown(destIP.ToString()),destPortNo);
      sThread->start();
  }
  
  virtual void startRcvr(int listnerPort) override
  {
	  rThread = new RcvrThread(pOutQ,listnerPort);
      rThread->start();
	  rThread->selfTerminate();
  }


  virtual void shutDownSender() override
  {
    postMessage("ChannelShutDown");
  }


  virtual Message^ getMessage() override
  {
    stdMessage smsg = pOutQ->deQ();//wait on queue until you get a new message!!!
 
	return ConvertMsgUp(smsg);
  }
  

  virtual void postMessage(Message^ msg) override
  {
    stdMessage smsg = ConvertMsgDown(msg);
    pInQ->enQ(smsg);
  }
 

  void showQueues(const std::string& msg)
  {
    sout << locker << "\n  " << msg << unlocker;
    sout << locker << "\n inQ has " << pInQ->size() << " messages";
    sout << " and outQ has " << pOutQ->size() << " messages" << unlocker;
    sout.flush();
  }

  virtual void setDestAddress(String^ ip,String^ port) override
  {
	  destIP.Append(ip);
	  destPortNo=Int32::Parse(port);
  }

  virtual Message^ getIPAddress() override
  {
	  SocketSystem  s1;
	  return  ConvertMsgUp(s1.getIpFromName(s1.getHostName())); 
  }


   stdMessage returnFileName(std::string fqfn)
  {
	  std::string output;
	  for(size_t i=fqfn.find_last_of("\\")+1;i<fqfn.length();i++)
	  {
		  output.push_back(fqfn[i]);
	  }
	  return output;
  }

  virtual void sendFile(String^ fileName) override
  {
	  fileChunks fchunks=fileChunkerDechunker::getBlockFromFile(ConvertMsgDown(fileName));
	 
	  pInQ->enQ("PUSH-File|"+returnFileName(ConvertMsgDown(fileName)));


	  for(auto chunk : fchunks)
	  {
		  //push chunks to the inputQ where the messages will be sent to the destination server
		  stdMessage x=Base64::encode(chunk);//file is sent in encoded form 
		  pInQ->enQ(x);
	  }

	  pInQ->enQ("EOF");

  }




private:
	  BQueue* pInQ;
	  BQueue* pOutQ;
	
	  Socket *sender;

	  System::Text::StringBuilder destIP;
	  int destPortNo;
		
	  SendingThread* sThread;
	  RcvrThread * rThread;
};


ICommChannel^ ICommChannel::CreateChannel()
{
  return gcnew CommunicationChannel();
}


/*
int main()
{
  Message^ testMessage = "testMessage";
  
  stdMessage sTestMessage = ConvertMsgDown(testMessage);
  
  std::cout << "\n  " << sTestMessage;
  
  Message^ resultMessage = ConvertMsgUp(sTestMessage);
  
  Console::Write("\n  {0}\n", resultMessage);

  ICommChannel^ chan = ICommChannel::CreateChannel();

  Message^ msg = "a message";
  
  chan->postMessage(msg);
  
  Message^ retMsg = chan->getMessage();
  
  Console::Write("\n  {0}\n\n", retMsg);
  
  chan->shutDown();
}

*/