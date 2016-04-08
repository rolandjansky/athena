
#include "TSystem.h"

void egammaValidationRunMacros()
{
  int nbTests=6;
  string macros[]={"'egammaValidation_hist.C\(\"ntuple.root\"\)'",
		   "'egammaValidationHistsToPostScript.C\(\"cluster_validation.hist\",\"refFile_cluster_validation.hist\",\"cluster_validation.ps\"\)'",
		   "'egammaValidationHistsToPostScript.C\(\"softE_validation.hist\",\"refFile_softE_validation.hist\",\"softE_validation.ps\"\)'",
		   "'egammaValidationHistsToPostScript.C\(\"eGamma_validation.hist\",\"refFile_eGamma_validation.hist\",\"eGamma_validation.ps\"\)'",
		   "'egammaValidation_ID_hist.C\(\"ntuple.root\"\)'",
		   "'egammaValidationHistsToPostScript.C\(\"egammaID_validation.hist\",\"refFile_egammaID_validation.hist\",\"egammaID_validation.ps\"\)'"
  };

  ofstream chkfile("runMacros.txt",ios::out);	
  
  for(int i=0;i<nbTests;i++) {
    string com;
    chkfile << "Run " << macros[i] << std::endl;
    com="root.exe -b -q "+macros[i];
    gSystem->Exec(com.c_str());
    chkfile << "Run " << macros[i] << " Done." << std::endl;
  }
  chkfile.close();
}
