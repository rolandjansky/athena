
#include "TSystem.h"

void CaloTests_runMacros2()
{
  int nbTests=2;
  string macros[]={"CaloTests_runPython2.C","CaloTests_plotAllCaloNoise13.C"};

  ofstream chkfile("CaloTests_runMacros2.txt",ios::out);	
  

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
