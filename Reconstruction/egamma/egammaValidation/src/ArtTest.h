#ifndef egammaValidation_ArtTest_H
#define egammaValidation_ArtTest_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

#include "TFile.h"
#include "TH1.h"

#include <string>

using namespace std;

class ArtTest : public AthAlgorithm
{
 public:

  /// Tools and services ///
  ITHistSvc*    rootHistSvc;

  // Histos
  // General Info
  TH1D *m_evtNmb = 0; //!

  // Electron
  TH1D *m_pT_ElTrk_All = 0; //!
  TH1D *m_pT_ElTrk_LLH = 0; //! 
  TH1D *m_pT_ElTrk_MLH = 0; //!
  TH1D *m_pT_ElTrk_TLH = 0; //!

  ArtTest (const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual StatusCode initialize ();
  virtual StatusCode beginInputFile();
  virtual StatusCode firstExecute();
  virtual StatusCode execute ();
  virtual StatusCode finalize ();

 private:

  AsgElectronLikelihoodTool* m_LooseLH;
  AsgElectronLikelihoodTool* m_MediumLH;
  AsgElectronLikelihoodTool* m_TightLH;

};

#endif
