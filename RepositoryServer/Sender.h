/////////////////////////////////////////////////////////////////
// Sender.cpp - Demonstration of concurrent socket connectors  //
// ver 2                                                       //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2013   //
/////////////////////////////////////////////////////////////////
/*
 * This Sender expects to write lines of text only.
 * So message framing is done by lines.  
 *
 * For HTTP like protocols the Sender should send lines for each
 * header attribute and bytes in the body, if there is one,
 * specified by a last header line something like:
 *    content_length : 1024
 * where 1024 is a stand-in for whatever you want your block 
 * size to be.
 *
 */

/*
 * Required files:
 * - Sender.cpp, Sockets.h, Sockets.cpp, 
 *   Threads.h, Threads.cpp, Locks.h, Locks.cpp
 *   BlockingQueue.h, BlockingQueue.cpp
 *
 * Maintanence History:
 * ver 1.1 - 30 Mar 2013
 * - changed Sendthread from terminating to default
 * - minor changes to error handling
 * ver 1.0 - 29 Mar 2013
 * - first release
 */

#include "Sockets.h"
#include "Threads.h"
#include "Locks.h"
#include "BlockingQueue.h"
#include <string>
#include <iostream>
#include <sstream>

///////////////////////////////////////////////////
// SendThread thread

class SendThread : public threadBase
{
public:
  SendThread(Socket s, BlockingQueue<std::string>& q) : s_(s), q_(q) {}
  std::string& status() { return status_; }
private:
  void run()
  {
    status_ = "good";
    doLog("send thread running");
    std::string msg;
    do
    {
      doLog("send thread enqing msg");
      msg = q_.deQ();
		
      if(!s_.writeLine(msg))
      {
        sout << "\n  bad status in sending thread";
        status_ = "bad";
        break;
      }
    } while(msg != "stop");
    s_.disconnect();
  }
  std::string status_;
  Socket s_;
  BlockingQueue<std::string>& q_;
};

std::string ToString(int i)
{
  std::ostringstream conv;
  conv << i;
  return conv.str();
}

class Sender
{
public:
  
  Sender(int numMsgs) : numMsgs_(numMsgs) { myCount = ++count; }
  
  int id() { return myCount; }

  void start(std::string ip, int port)
  {
    
	sout << locker << "\n Sender #" << id() << " started" << unlocker;
    pSt = new SendThread(s_, q_);
    pSt->start();
    
	if(!s_.connect(ip, port))
    {
      sout << locker << "\n  couldn't connect to " << ip << ":" << port << "\n\n" << unlocker;
      delete pSt;
      return;
    }
    
	else
    {
      std::string logMsg = "\n  connected to " + ip + ":" + ToString(port);
      doLog(logMsg.c_str());
    }
  
	doLog("starting Sender");
    std::string msg;

    for(int i=0; i<numMsgs_; ++i)
    {
      doLog("sending message");

	  msg = "sender#" + ToString(id()) + ": msg#" + ToString(i);
  
	  sout << locker << "\n  " << msg.c_str() << unlocker;
      
	  q_.enQ(msg);
      
	  ::Sleep(10 * id());  // sleep time increases with each addition Sender
      
	  if(pSt->status() == "bad")
        break;
    
	}


	q_.enQ(std::string(msg+"stop").append(ToString(2)));

    msg = "sender#" + ToString(id()) + ": stop";
    sout << "\n  " + msg;
    pSt->join();
    delete pSt;
  }

private:
  Socket s_;
  BlockingQueue<std::string> q_;
  SendThread* pSt;
  static int count;
  int myCount;
  int numMsgs_;
};

int Sender::count = 0;

///////////////////////////////////////////////////
// DemoThread is used to get two or more senders
// running concurrently from a single process, to
// make testing easier.

class DemoThread : public threadBase
{
public:
  DemoThread(Sender sndr) : sndr_(sndr) {}
private:
  void run()
  {
    sndr_.start("127.0.0.1", 8080);
  }
  
  Sender sndr_;

};


//
//int main()
//{
//  int ret = 0;
//  try
//  {
//    // run two senders concurrently
//    Sender sndr1(100), sndr2(100);
//   
//	DemoThread t1(sndr1);
//    
//	DemoThread t2(sndr2);
//    
//	t1.start();
//    t2.start();
//    t1.join();
//    t2.join();
//  }
//
//  catch(std::exception& ex)
//  {
//    sout << "\n\n  " << ex.what();
//    ret = 1;
//  }
//  catch(...)
//  {
//    sout << "\n\n  something bad happend to a sender";
//    ret = 1;
//  }
//  std::cout << "\n\n  That's all Folks!\n\n";
//  return ret;
//}
