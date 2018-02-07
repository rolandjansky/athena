/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef egammaValidation_EgammaMonitoring_H
#define egammaValidation_EgammaMonitoring_H

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

class EgammaMonitoring : public AthAlgorithm
{
 public:

  /// Tools and services ///
  ITHistSvc*    rootHistSvc;

  // Histos
  // General Info
  TH1D *evtNmb = 0; //!

  // Electron
  TH1D *pT_ElTrk_All  = 0; //!
  TH1D *pT_ElTrk_LLH  = 0; //! 
  TH1D *pT_ElTrk_MLH  = 0; //!
  TH1D *pT_ElTrk_TLH  = 0; //!
  TH1D *eta_ElTrk_All = 0; //!
  TH1D *phi_ElTrk_All = 0; //!
  
  // Photon
  TH1D *pT_Phot_All  = 0; //!
  TH1D *eta_Phot_All = 0; //!
  TH1D *phi_Phot_All = 0; //!
  
  EgammaMonitoring (const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual StatusCode initialize ();
  virtual StatusCode beginInputFile();
  virtual StatusCode firstExecute();
  virtual StatusCode execute ();
  virtual StatusCode finalize ();

 private:

  /// Sample name ///
  string m_sampleType;

  AsgElectronLikelihoodTool* m_LooseLH;
  AsgElectronLikelihoodTool* m_MediumLH;
  AsgElectronLikelihoodTool* m_TightLH;

};

#endif
