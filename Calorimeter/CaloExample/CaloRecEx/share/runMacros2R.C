
#include "TSystem.h"

void runMacros2R()
{
  int nbTests=2;
  string macros[]={"CaloRecEx_dump2.C","runPython2R.C"};

  ofstream chkfile("runMacros2R.txt",ios::out);	
  
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
