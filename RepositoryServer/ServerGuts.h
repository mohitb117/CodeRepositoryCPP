///////////////////////////////////////////////////////////////////////////////////////////////
// ServerGuts.h -This is the parent class which initializes server data and functionality    //                                                                              //
//																							 //
// Language:    Visual C++,														             //
// Platfrom:    Lenovo U410 Ultrabook Win7x64									             //
// Application: CSE687 - Object Oriented Design, Spring 2013                                 //
// Author:		Mohit Bhalla Syracuse University					                         //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                                  //
//              (315) 443-3948, jfawcett@twcny.rr.com                                        //
//////////////////////////////////////////////////////////////////////////////////////////////

# include "XmlReader.h"
# include "XmlWriter.h"

# include "Reciever.h"
# include "FileSystemX.h"

using namespace FileSystem;


class ServerGuts
{
public:

	void createUserInfoFile(std::string fName)
	{
				  XmlWriter wtr;
				 
				  XmlWriter body,sub1;
				  wtr.addDeclaration();
    			  wtr.addComment("User-Information");
  			      wtr.start("root");  
				  sub1.start("User /");
								  sub1.addAttribute("UserName","mohit");  
								  sub1.addAttribute("Password","bhalla");  
				 wtr.addBody(sub1.xml());
				 sub1.clear();
								  sub1.start("User /");
								  sub1.addAttribute("UserName","jim");  
								  sub1.addAttribute("Password","fawcett");  
				
			      wtr.addBody(sub1.xml());
				  sub1.clear();
								  sub1.start("User /");
								  sub1.addAttribute("UserName","mehmet");  
								  sub1.addAttribute("Password","kaya");  
			      wtr.addBody(sub1.xml());
				  sub1.clear();
								  sub1.start("User /");
								  sub1.addAttribute("UserName","vicky");  
								  sub1.addAttribute("Password","singh");  
				  wtr.addBody(sub1.xml());
				  sub1.clear();
								  sub1.start("User /");
								  sub1.addAttribute("UserName","elie");  
								  sub1.addAttribute("Password","elaaraj");  
				  wtr.addBody(sub1.xml());
				  wtr.end();
				  std::ofstream out(fName+".xml");
				  if(out.good())
				  {
					out << wtr.xml().c_str();
					out.close();
				  }
	}

	void initializeUserInfo()
	{
			if(!Directory::exists("ServerRepo"))//if directory doesnt exist!
			{				
				Directory::create("ServerRepo");
				std::string curr=Directory::getCurrentDirectory();
				
				createUserInfoFile(curr+"\\ServerRepo\\"+"userInfo");
			}

			else
			{
				std::string curr=Directory::getCurrentDirectory();
				createUserInfoFile(curr+"\\ServerRepo\\"+"userInfo");
			}

	}

	ServerGuts(int x):portNo(x)
	{
		/*
				Actions for the server:-
				1. Start the listner at the port   DONE
				2. Create a Directory at the current Location  DONE
				3. Create an XML file with User Information  DONE
		*/
		r1=new Receiver();
		initializeUserInfo();
	}



	bool startServer()
	{
		//start the listner Thread	
		
		if(portNo>0)
		{	
			SocketSystem s1;
			std::cout<<"\n Starting Repository Server at Address "<<s1.getIpFromName(s1.getHostName())<<":"<<portNo<<"\n" ;
			
			std::cout<<"\n Current Working Directory of Repository Server =>\n"<<Directory::getCurrentDirectory()+"\\ServerRepo\\"<<"\n" ;
			
			r1->start(portNo);//start the listner at the port Numbr
			
			return true;
		}


			return false;
	}


private:
	int portNo;
	Receiver * r1;
};

