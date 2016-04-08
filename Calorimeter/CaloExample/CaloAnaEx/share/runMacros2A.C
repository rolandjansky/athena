
#include "TSystem.h"

void runMacros2A()
{
  int nbTests=1;
  string macros[]={"runPython2A.C"};

  ofstream chkfile("runMacros2A.txt",ios::out);	
  
  for(int i=0;i<nbTests;i++)
    {
      string com;
      chkfile << "Run " << macros[i] << std::endl;
      com="root.exe -b -q "+macros[i];
      gSystem->Exec(com.c_str());
      chkfile << "Run " << macros[i] << " Done." << std::endl;
    }
  chkfile.close();
}
