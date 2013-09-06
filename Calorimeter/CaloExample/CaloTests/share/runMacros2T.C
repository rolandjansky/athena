
#include "TSystem.h"

void runMacros2T()
{
  int nbTests=2;
  string macros[]={"runPython2T.C","CaloTests_plotAllCaloNoise13.C"};

  ofstream chkfile("runMacros2T.txt",ios::out);	
  

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
