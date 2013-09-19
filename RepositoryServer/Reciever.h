//////////////////////////////////////////////////////////////////////////////////
// Reciever.h - Demonstration of socket reciever with							//
//                concurrent clients											//
//																				//
// Language:    Visual C++,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////
#include "Sockets.h"
#include "Threads.h"
#include "Locks.h"
#include "BlockingQueue.h"
#include <string>
#include <iostream>
# include <sstream>
# include "XmlReader.h"
# include "FileSystemX.h"
# include "FileChunker.h"
# include "Base64.h"
using namespace FileSystem;
using namespace WinTools_Extracts;
///////////////////////////////////////////////////
// ClientHandlerThread thread

std::string ToString(int i)
{
  std::ostringstream conv;
  conv << i;
  return conv.str();
}

class ClientHandlerThread : public tthreadBase
{ 
public:
	ClientHandlerThread(Socket s, BlockingQueue<std::string>& q) : s_(s), q_(q) 
	{
			isAuth=isGetStatus=isPush=false;
	}

private:  

	bool verifyUserNamePassword(std::string key,std::string value)
	{
		std::string curr=Directory::getCurrentDirectory();
		
		std::ifstream t(curr+"\\ServerRepo\\userInfo.xml");

		std::stringstream buffer;
		buffer << t.rdbuf();
		std::string contents=buffer.str();

		XmlReader rdr(contents);
		std::string userName,password;
		
		  while(rdr.next())
		  {
			/*sout << "\n  tag:     " <<*/ rdr.tag().c_str();
			/*sout << "\n  element: " <<*/ rdr.element().c_str();
			/*sout << "\n  body:    " <<*/ rdr.body().c_str();
		
			XmlReader::attribElems attribs = rdr.attributes();
			
			for(size_t i=0; i<attribs.size(); ++i)
			{
				//sout << "\n  attributes: " << attribs[i].first.c_str() << ", " << attribs[i].second.c_str();

				if(std::strcmp(attribs[i].first.c_str(),"UserName")==0)
				{
					userName=attribs[i].second.c_str();
				}

				else if(std::strcmp(attribs[i].first.c_str(),"Password")==0)
				{
					password=attribs[i].second.c_str();
				}

				if(userName==key && password==value)
				{
					return true;
				}
		

			}
			 
		
		  }

		  return false;
	}
	  /*

  Authentication  msg
  line 1    AUTH       ctr=0
  line 2    IP:Port        1
  line 3    User:Pass      2
  line 4    EOA            3
  ----------------------------------------------------
  Push Pkg  msg
  line 1    PushPkg       ctr=0
  line 2    IP:Port        1

  line 3    File:<Name>    2
  line x    Chunks         x
  line 4    EOFIle         n  <== Dump FIle to Repository

  Line x+n   EOPushPkg
  -----------------------------------------------
  GetStatus  msg
  line 1    GStatus       ctr=0
  line 2    IP:Port        1
  line 3    User:Pass      2
  line 4    EOStatus       3

  */

	void createPackageOwner(std::string fName,std::string packageName,std::string usrName)
	{
				  XmlWriter wtr;
				  XmlWriter body,sub1;
				  wtr.addDeclaration();
    			  wtr.addComment("Package-Information");
  			      wtr.start("Package");
				  wtr.addAttribute("Package-Name",packageName);

				  sub1.start("User /");

				  sub1.clear();
								  sub1.start("User /");
								  sub1.addAttribute("UserName",usrName);  
				  wtr.addBody(sub1.xml());

				  wtr.end();
				  std::ofstream out(fName+".xml");
				  if(out.good())
				  {
					out << wtr.xml().c_str();
					out.close();
				  }
	}


   void scanMessage(std::string messageType,int &ctr)
   {
	   if(messageType=="AUTH" && ctr ==0)
	   {
		   isAuth=true;
	   }

	   else if(messageType=="Get-Status" && ctr==0)
	   {
		   isGetStatus=true;
	   } 
	
	   else if (messageType=="PUSH-PKG" && ctr==0)
	   {
		   isPush=true;
	   }

   }


   void handleAuthentication(std::string messageType,int & ctr)
   {
	   if(messageType.find(":")>0 && messageType.find(":")<=INT_MAX && ctr==1)
	   {
	   		   ipAddress=messageType.substr(0,messageType.find(":"));		  
			   destPort=atoi(messageType.substr(messageType.find(":")+1,messageType.length()).c_str());				  
	   }

	   if(messageType.find(":")>0 && messageType.find(":")<=INT_MAX && ctr==2)
	   {
			   username=messageType.substr(0,messageType.find(":"));		  
			   password=(messageType.substr(messageType.find(":")+1,messageType.length()).c_str());				  
	   }

	   else if(messageType=="EOA")
 	   {	
   						  Socket *Ack=new Socket();  
					
						  Ack->connect(ipAddress,destPort);

						  if(verifyUserNamePassword(username,password))
						  {
							    Ack->writeLine("YES");
						  }
				
						  else
						  {
							    Ack->writeLine("NO");
						  }

						  Ack->disconnect();

		                  isAuth=isGetStatus=isPush=false;

			ctr=-1;
	   }
   }

   void handleVersionDirectoryCreation(std::string & messageType)
   {
   		   //check if package exists in the directory
		   //if yes, then just add files to the directory
		   //else 
		   //create package with name of message Type
		   std::string currDirectory=Directory::getCurrentDirectory();
		   currNamespaceName=messageType.substr(0,messageType.find("|"));
		   currPackageName=messageType.substr(messageType.find("|")+1,messageType.length());

		   currNamespaceDirectory=currDirectory+"\\ServerRepo\\"+currNamespaceName;
		   currentPkgDirectory=currNamespaceDirectory+"\\"+currPackageName;
		   int count =Directory::getDirectories(currentPkgDirectory).size()+1;
		  
		   currPkgVersion=currentPkgDirectory+"\\"+
						    +"V"
			                +ToString(count)
			                +std::string(".")
							+ToString(0);
		   
		   //if directories doesnt exist, create it
		   if(!Directory::exists(currNamespaceDirectory) )		  	 
		 	   Directory::create(currNamespaceDirectory);
		  
		   if(!Directory::exists(currentPkgDirectory) )
			   Directory::create(currentPkgDirectory);

		    Directory::create(currPkgVersion);
		  //initialize the package XML file as well	
			if(!FileSystem::File::exists(currentPkgDirectory+"\\OwnerMetaData.xml"))//if metadata file doesnt exist, create it
			{
				createPackageOwner(currentPkgDirectory+"\\OwnerMetaData.xml",currPackageName,username);
			}
   }
   void handlePushPkg(std::string messageType,int & ctr)
   {
	   if(messageType=="PUSH-PKG")
	     ;
	   
	   else if(ctr==1)
	      handleVersionDirectoryCreation(messageType); 
	   
	   if(ctr>2 && messageType!="EOF")
	   {
		   fileDissassembled.push_back(convertVectorChar2String(Base64::decode(messageType)));//store file chunks in temporary store
	   }
	   if(messageType.substr(0,messageType.find("|"))=="PUSH-File")
	   {
		   fileDissassembled.clear();
		   currFileName=messageType.substr(messageType.find("|")+1,messageType.length());
	   }

	   if(ctr>2 && messageType=="EOF")
	   { //dump file fragments in temporary store
		   WinTools_Extracts::fileChunkerDechunker::writeBlocks2File(fileDissassembled,currPkgVersion+"\\"+currFileName);
		  fileDissassembled.clear();
	   }

	   if(messageType=="EOPUSH-PKG")
	   {				  Socket *Ack=new Socket();  
						  Ack->connect(ipAddress,destPort);
						  Ack->writeLine("Package "+currPackageName+" Successfully Stored in Repository");
						  Ack->disconnect();
						  isAuth=isGetStatus=isPush=false;
						  ctr=-1;
	    				  fileDissassembled.clear();
	   }
   }


  void run()
  {
    std::string msg;
	int ctr=0;
	fileDissassembled.clear();
	do
    {
      doLog("receive thread reading line");
	  msg = s_.readLine();
	   if(msg == "")
		  continue;

      if(msg!="" && !isAuth && !isGetStatus && !isPush)//first determine message type
	  {		  scanMessage(msg,ctr);   }
	  if(isAuth && !(isGetStatus && isPush))//the consequent message fragements are AUTH
	  {		  handleAuthentication(msg,ctr);   }
	  if(isPush && !(isGetStatus&&isAuth))//the consequent message fragements are Push
	  {		  handlePushPkg(msg,ctr) ;  }

	   if(isGetStatus && !(isAuth && isPush)) //the consequent message fragements are Status Req
	  {  }
	   q_.enQ(msg);

	  ctr++;
    } 
	while(msg != "KillClientHandler");
  }

  /*
             * The simple idea, select 2 files , give a namespace id to them
             * then for each file, 
             * 
             * Outer Message:-
             *      
             *          PUSH-PKG  
             *          PkGNAME   <== Check if directory exists, else create a new directory,
             *          
             *          PUSH-File     
			 *			FileName  <== create filehandle 
             *          File Contents
             *          EOF 
             *          
             *          PUSH-File     
			 *			FileName
             *          File Contents
             *          EOF 
             *          
             *          PUSH-File
             *          File Contents
             *          EOF 
             *          
             *          EOPush
             */


  /*
  Additionally I need when a package is dumped in the repository , to 
  create a xml file per package and add owner information to the package

  if new code is being added to old package 
  create a new version folder and set the current version in the xml file as the latest version
  
  */

  std::string currentPkgDirectory,currNamespaceDirectory;
  std::string currPackageName,currNamespaceName;
  std::vector<std::string> fileDissassembled;
  std::string currFileName,currPkgVersion;
   std::string username,password,ipAddress;
   int destPort;
  Socket s_;
  BlockingQueue<std::string>& q_;
  bool isAuth,isPush,isGetStatus;
};

///////////////////////////////////////////////////
// listenThread thread
class ListenThread : public threadBase
{
public:
 
  ListenThread(int port, BlockingQueue<std::string>& q) : listener_(port), q_(q), stop_(false) {}
  
  void stop(bool end) { stop_ = end; }

private:

  void run()
  {

    while(!stop_)
    {
      SOCKET s = listener_.waitForConnect();
     
	  ClientHandlerThread* pCh = new ClientHandlerThread(s, q_);	  //code needs changing for client handler's run function!

	  pCh->start();
    }

  }
  bool stop_;
  BlockingQueue<std::string>& q_;
  SocketListener listener_;
};

class Receiver
{
public:

  void start(int port)
  {
	sout << "Reciever started at Port # "<<port<<"\n";
   
	pLt = new ListenThread(port, q_);
    
	try
    {
      pLt->start();
      std::string msg;
      int count = 0;
      int ctr=0;
	  bool flag=true;
	  do
      {
		msg = q_.deQ();
		doLog((std::string("fetching message ") + msg).c_str());
    
		if(msg.length()<=20)
			sout <<locker<< "\n  Recieved from " << msg.c_str()<<unlocker;
		else
						sout <<locker<< "\n  File Chunk Recieved ";
		sout<<locker<<" "<<ctr++<<" "<<unlocker;
		if(msg=="KillListner")
			flag=false;
	  } 
	  while(flag);
	  if(!flag)
	  {
			pLt->stop(1);
			std::cout<<"\nKilling listener thread!!\n";		
	  }
    }
    catch(std::exception& ex)
    {
	  sout<<ex.what()<<"\n";
      delete pLt;
    }
    catch(...)
    {
      delete pLt;
    }

  }
private:
  BlockingQueue<std::string> q_;
  ListenThread* pLt;
};


