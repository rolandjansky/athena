
#include "TSystem.h"

void CaloAnaEx_runMacros2()
{
  int nbTests=1;
  string macros[]={"CaloAnaEx_runPython2.C"};

  ofstream chkfile("CaloAnaEx_runMacros2.txt",ios::out);	
  
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
