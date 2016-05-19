#include "TROOT.h" 
#include "TTree.h" 
#include "TCanvas.h" 
#include <iostream>


#include <fstream>
#include <string>



string findRef()
{
  char line[999];
  FILE *f=gSystem->OpenPipe("grep RTTINFO CaloRec_RTT*_log | cut -d= -f2 | tail -n1","r");
  if(f)
    {
      fgets(line,999,f);
      line[strlen(line)-1] = 0;
    }
  
  if(-1 == fclose(f)){
    std::cerr<<"can not close file"<< std::endl;
  }

  string temp(line);
  string temp2="";
  for(int i=0;i<temp.size();i++)
    if (temp[i]!=' ')
      temp2+=temp[i];

  return temp2;
}

void CaloRec_truth()
{




  string RTTinfo=findRef();

  int type;
  if ( strstr(RTTinfo.c_str(),"Photons")!=0 )
    {
      type=1;
      std::cout << "Photons input" << std::endl;
    }
  else
    if ( strstr(RTTinfo.c_str(),"Electrons")!=0 )
      {
	type=2;
	std::cout << "Electrons input" << std::endl;
      }  
    else
      if ( strstr(RTTinfo.c_str(),"H_2e2mu")!=0 )
	{
	  type=3;
	  std::cout << "H2e2mu input" << std::endl;
	}
      else
	if ( strstr(RTTinfo.c_str(),"Zee")!=0 )
	  {
	    type=4;
	    std::cout << "Zee input" << std::endl;
	  }
	else
	if ( strstr(RTTinfo.c_str(),"top")!=0 )
	  {
	    type=5;
	    std::cout << "Top input" << std::endl;
	  }
	else
	  return;
      
  // gInterpreter->ProcessLine(".L t3333.C+");
gROOT->ProcessLine(".L t3333.C+");

  gSystem->Load("t3333_C");


      
  t3333 t; 
  // loop over events, fill histo
  t.Loop(type); 
 
  // t3333 does not own tree. 
  t.Init(0); 
   
}
