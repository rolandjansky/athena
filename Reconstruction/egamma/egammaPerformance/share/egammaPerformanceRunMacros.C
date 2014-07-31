
#include "TSystem.h"

void egammaPerformanceRunMacros()
{
  int nbTests=6;
  string macros[]={"'egammaPerformance_hist.C\(\"ntuple.root\"\)'",
		   "'egammaPerformanceHistsToPostScript.C\(\"cluster_validation.hist\",\"refFile_cluster_validation.hist\",\"cluster_validation.ps\"\)'",
		   "'egammaPerformanceHistsToPostScript.C\(\"softE_validation.hist\",\"refFile_softE_validation.hist\",\"softE_validation.ps\"\)'",
		   "'egammaPerformanceHistsToPostScript.C\(\"eGamma_validation.hist\",\"refFile_eGamma_validation.hist\",\"eGamma_validation.ps\"\)'",
		   "'egammaPerformance_ID_hist.C\(\"ntuple.root\"\)'",
		   "'egammaPerformanceHistsToPostScript.C\(\"egammaID_validation.hist\",\"refFile_egammaID_validation.hist\",\"egammaID_validation.ps\"\)'"
  };

  ofstream chkfile("runMacros.txt",ios::out);	
  
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
