
#include "TSystem.h"

void runMacros()
{

  string macros[]={"CaloRecEx_dump.C","CaloRecEx_histComp.C","CaloRecEx_truth.C","checkPOOL.C"};

  string ldpath=gSystem->Getenv("LD_LIBRARY_PATH");
  ldpath="/afs/cern.ch/sw/lcg/external/root/5.10.00c/slc3_ia32_gcc323/root/lib"+ldpath;
  gSystem->Setenv("LD_LIBRARY_PATH",ldpath.c_str());

   string path=gSystem->Getenv("PATH");
   path="/afs/cern.ch/sw/lcg/external/root/5.10.00c/slc3_ia32_gcc323/root/bin"+path;
   gSystem->Setenv("PATH",path.c_str());


  for(int i=0;i<4;i++)
    {
      string com;
      std::cout << "Run " << macros[i] << std::endl;
      com="/afs/cern.ch/sw/lcg/external/root/5.10.00c/slc3_ia32_gcc323/root/bin/root.exe -b -q "+macros[i];
      gSystem->Exec(com.c_str());
      std::cout << "Run " << macros[i] << "Done." << std::endl;
    }

}
