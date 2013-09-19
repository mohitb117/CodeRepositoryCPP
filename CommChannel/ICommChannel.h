//////////////////////////////////////////////////////////////////////////////////
// ICommChannel.h - This file contains the interface for use by the C# code,	//
//							acting as a C++/CLI Bridge between					//
//										the managed and unmanaged code			//
//																				//
//The Peer Threads (i.e. Sender, reciver threads								//
//      use the Communication channel BQ in order to Send/Receive messages)     //	
//																				//
// Language:    Visual C++,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////

# include "Sockets.h"
# include "BlockingQueue.h"
# include "FileSystem.h"
# include "FileChunker.h"
# include "Base64.h"

#ifndef CommChannel_H

#define CommChannel_H

namespace CommChannel
{
	using namespace System;
	
	typedef String Message;

	public ref class  ICommChannel abstract
	{
		
		public:
			static ICommChannel^ CreateChannel();
			virtual Message^ getMessage()=0;
			virtual void postMessage(Message^ msg)=0;
			virtual void shutDownSender()=0;

			virtual void setDestAddress(String^ ip,String^ port)=0;
			
			virtual void startSndr()=0;

			virtual void startRcvr(int portNo)=0;
			
			virtual Message^ getIPAddress()=0;

			virtual void sendFile(String^ fileName)=0;//takes fileName, breaks it to chunks and sends it to the server
	
	};
}




#endif
