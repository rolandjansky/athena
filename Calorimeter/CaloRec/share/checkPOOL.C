#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "TTree.h"
#include "TString.h"
#include "TCanvas.h"
#include "TKey.h"
#include "TFile.h"
#include "TStreamerInfo.h"

void PrintFileInfo(TFile * f);

void checkPOOL()
{

char com[500];
  sprintf(com,"rm checkPOOL.txt");
  gSystem->Exec(com);

  char line[999];
  FILE *fich=gSystem->OpenPipe("ls *pool.root","r");
  if(fich)
    {
      do
	{
	  sprintf(line,"");
	  fgets(line,999,fich);
	   if (strlen(line)>0)
	  	      line[strlen(line)-1] = 0;
	  else
	    return;
	 
	   ofstream chkfile("checkPOOL.txt",ios::app);

	   TFile * f = TFile::Open(line);
	  PrintFileInfo(f);
	  TIter next(f->GetListOfKeys());
	  TKey *key;
	  chkfile.width(7); chkfile << " Events";
	  chkfile.setf(std::ios::right);
	  chkfile.setf(std::ios::fixed);

	  chkfile.width(20); chkfile << "Size(mem) kB"; 
	  chkfile.width(20); chkfile << "Size(zip) kB"; 
	  chkfile.unsetf(std::ios::right);
	  chkfile.unsetf(std::ios::fixed);
	  chkfile << "\t\t Name of Object" << std::endl; 

	  while ((key=(TKey*)next()))
	    {
	      key=(TKey*)next();
	     	      TTree * tt = (TTree*)key->ReadObj();
	      TString tempName=key->GetName();
	      if (tt!=0 &&  tempName.Sizeof()>12 )  
		{
		  TString keyName=tempName(14,100);
		  chkfile.width(7); chkfile << tt->GetEntries();
		  chkfile.setf(std::ios::right);
		  chkfile.setf(std::ios::fixed);
		  chkfile.precision(3);
		  chkfile.width(20); chkfile << tt->GetTotBytes()/1000.;
		  chkfile.width(20); chkfile << tt->GetZipBytes()/1000.;
		  chkfile.unsetf(std::ios::right);
		  chkfile.unsetf(std::ios::fixed);
		  chkfile << "\t\t" << keyName << std::endl;
		}
	    }
	 
	  f->Close();
	  chkfile.close();
	}
   while ( strlen(line)>0)
     }
  if(-1 == fclose(fich))
    {
      std::cerr<<"can not close file"<< std::endl;
    }
  return;
}
      
void PrintFileInfo(TFile * f)
{
ofstream chkfile("checkPOOL.txt",ios::app);
  chkfile << "File " << f->GetName() << " has " << f->GetSize() << " Bytes." << std::endl;
  chkfile << "Compression factor is " << f->GetCompressionFactor();
  chkfile << " and a compression level of " << f->GetCompressionLevel() << " " << std::endl;
  chkfile << std::endl; 
  chkfile.close();
  return;
}
