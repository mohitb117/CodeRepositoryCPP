///////////////////////////////////////////////////////////////////////////////////////////////
// ServerExec.cpp -The Execution Module of the Code Repository Server where the				 //
//												execution of the server threads start		 //                                                                          
//																							 //
// Language:    Visual C++,														             //
// Platfrom:    Lenovo U410 Ultrabook Win7x64									             //
// Application: CSE687 - Object Oriented Design, Spring 2013                                 //
// Author:		Mohit Bhalla Syracuse University					                         //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                                  //
//              (315) 443-3948, jfawcett@twcny.rr.com                                        //
///////////////////////////////////////////////////////////////////////////////////////////////

#define ServerStart

#ifdef ServerStart

# include "ServerGuts.h"

int main(int argc , char** argv)
{
	if(argc>1)
	{
		ServerGuts *s1=new ServerGuts(atoi(argv[1]));
		s1->startServer();
	}
	
	else
	{
		ServerGuts *s1=new ServerGuts(4004);
		s1->startServer();
	}
	return 0;
}

#endif