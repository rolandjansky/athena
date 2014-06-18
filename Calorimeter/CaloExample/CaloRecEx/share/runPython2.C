
#include "TSystem.h"

void runPython2()
{
  int nbTests=1;
  string macros[]={"check.py"};
  ofstream chkfile("runPython2.txt",ios::out);	
  std::cout << "LD_LIBRARY_PATH is " << gSystem->Getenv("LD_LIBRARY_PATH") << std::endl;
  std::cout << "PATH is " << gSystem->Getenv("PATH") << std::endl;

  for(int i=0;i<nbTests;i++)
    {
      string com;
      chkfile << "Run " << macros[i] << std::endl;
      com="python "+macros[i];
      gSystem->Exec(com.c_str());
      chkfile << "Run " << macros[i] << " Done." << std::endl;
    }
  chkfile.close();

}


