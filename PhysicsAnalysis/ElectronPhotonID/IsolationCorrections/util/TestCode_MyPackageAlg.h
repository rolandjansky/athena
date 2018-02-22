#ifndef MYPACKAGE_MYPACKAGEALG_H
#define MYPACKAGE_MYPACKAGEALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

class MyPackageAlg: public ::AthAnalysisAlgorithm { 
 public: 
  MyPackageAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MyPackageAlg(); 
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  beginInputFile(); //start of each input file, only metadata loaded
  virtual StatusCode  execute();        //per event
  virtual StatusCode  finalize();       //once, after all events processed

 private: 
  Float_t pt_corrVal_cone20; //!
  Float_t pt_corrVal_cone40; //!
  Float_t dd_corrVal_cone20; //!
  Float_t dd_corrVal_cone40; //!

}; 

#endif //> !MYPACKAGE_MYPACKAGEALG_H
