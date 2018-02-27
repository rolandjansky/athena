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
#include <AsgTools/AnaToolHandle.h>
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

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
  TH1D *evtNmb = nullptr; //!

  // Electron
  TH1D *pT_ElTrk_All  = nullptr; //!
  TH1D *pT_ElTrk_LLH  = nullptr; //! 
  TH1D *pT_ElTrk_MLH  = nullptr; //!
  TH1D *pT_ElTrk_TLH  = nullptr; //!
  TH1D *eta_ElTrk_All = nullptr; //!
  TH1D *phi_ElTrk_All = nullptr; //!
  
  // Photon
  TH1D *pT_Phot_All  = nullptr; //!
  TH1D *eta_Phot_All = nullptr; //!
  TH1D *phi_Phot_All = nullptr; //!
  
  EgammaMonitoring (const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual StatusCode initialize ();
  virtual StatusCode beginInputFile();
  virtual StatusCode firstExecute();
  virtual StatusCode execute ();
  virtual StatusCode finalize ();

 private:

  /// Sample name ///
  std::string m_sampleType;

  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_LooseLH ; //!
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_MediumLH; //!
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_TightLH ; //!

};

#endif
