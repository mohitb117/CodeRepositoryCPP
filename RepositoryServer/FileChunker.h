//////////////////////////////////////////////////////////////////////////////////
// FileChunker.h - Represents Functionality for File Chnuking and Reconstruction//
//				   between peers									            //
// ver 1.2.0												                    //
//                                                                              //
// Language:    Visual C++,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
# include <sstream>
# include "FileSystem.h"

using namespace WinTools_Extracts;

std::string convertVectorStrings2String(std::vector<std::string> body)
{
			std::string result="";
			for(auto x:body)
			{
				result+=x;
			}
			return result;
}


std::string convertVectorChar2String(std::vector<char> body)
{
			std::string result="";
			for(auto x:body)
			{
				result+=x;
			}
			return result;
}

std::vector<char> convertString2CharVector(std::string body)
{
			std::vector<char> result;
			
			for(auto x:body)
			{
				result.push_back(x);
			}
			
			return result;
}


namespace WinTools_Extracts
{

	class fileChunkerDechunker
	{
	private:
		 std::vector<std::string> fChunks;
		  File * temp;

	public:
		static std::string convertBlock2String(File::block OutboundSocketData)
		{
				std::ostringstream conv;
					for(size_t i=0;i<OutboundSocketData.size();i++)
							conv<<OutboundSocketData[i];
		
				return conv.str();
		}


		static File::block convertString2Block(std::string IncomingSocketData)
		{
			File::block temp;
				for(auto ele:IncomingSocketData)
				{
					temp.push_back(ele);
				}
			 return temp;
		}

		static std::vector<File::block> getBlockFromFile(std::string fqfn,int blockSize=2048)//fqfn==>fully qualified filename
		{
			  File me(fqfn, File::in, File::binary);
			  std::vector<File::block> fileBroken;
		  
			  me.blockSize()=blockSize;

			  while(me.isGood())
			  {
				  fileBroken.push_back(me.getBlock());
			  }

	 		std::cout<<"Peer Successfully Chunked file of size "<<fileChunkerDechunker::filesize(fqfn)<<" (In bytes)"<<"\n";
	
		   return fileBroken;
		}


		static bool writeBlocks2File(std::vector<std::string> completeData,std::string FQFNfileName)
		{
				File destFile(FQFNfileName, File::out, File::binary);

					for(auto x:completeData)
					{
						if(destFile.isGood())
						{
							destFile.putBlock(convertString2Block(x));
						}

						else
							return false;
					}	
					std::cout<<"\nFile Successfully Written as "<<FQFNfileName<<"\n";
					return true;
			
		}


		static int filesize(std::string filename)
		{
				std::ifstream ifs; 
				ifs.open(filename, std::ios::binary); 
				ifs.seekg(0, std::ios::end); 
				return (int)ifs.tellg().seekpos();
		}


	///////////////////////////////////////////////////////////////////////



		std::vector<std::string> getFileChunks()
		{
			return fChunks;
		}

		 bool getChunks4mFile(std::string file,int chunkSize=1024)
		 {
			  std::ifstream fin;

			  fin.open(file, std::ios::binary);

			  std::ofstream fout;

			  if(!fin.good())
			  {
				std::cout << "\n  can't open file " << file.c_str() << "\n\n";
				return false;
			  }
	
			  char* buf=new char[chunkSize];

			  fout << fin.rdbuf();

			  int readCount=chunkSize;

			  while ( fin.read( &buf[0], chunkSize )  || (readCount = (int)fin.gcount()) != 0 ) 
	  		  {
						fout.write( &buf[0], readCount );
					
						buf[readCount-1]='\0';
							
						fChunks.push_back(buf);
	 		  }
				return true;
			}


		 static bool WriteChunks2File(std::vector<std::string> fileChunks,std::string fileName,std::string path)
		 {
			std::ofstream fout;

			fout.open(path+"/"+fileName, std::ofstream::binary | std::ofstream::out);

			  if(fout.is_open() && fileChunks.size()>0)
			  {   
				  for(auto str: fileChunks)
				  {
					  fout.write(str.c_str(), strlen(str.c_str())); 
					  fout<<"\n";
				  }
				 fout.close();
				 std::cout<<"Peer Received file of size "<<fileChunkerDechunker::filesize(path+"/"+fileName)<<" (In bytes)"<<"\n";
				 return true;
			  }

			  else
			  {
				  return false;
			  }
		  
		 }

	};
}