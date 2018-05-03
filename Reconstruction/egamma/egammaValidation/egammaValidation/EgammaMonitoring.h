/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef egammaValidation_EgammaMonitoring_H
#define egammaValidation_EgammaMonitoring_H

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <AsgTools/AnaToolHandle.h>
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "PATCore/AcceptData.h"

#include "ElectronPlots.h"
#include "PhotonPlots.h"
#include "ShowerShapesPlots.h"
#include "EffIDPlots.h"
#include "EffRecPlots.h"

#include "TFile.h"
#include "TH1.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std;

class EgammaMonitoring : public AthAlgorithm
{
 public:

  /// Tools and services ///
  ITHistSvc*    rootHistSvc;

  egammaMonitoring::EffIDPlots  Eff_ID  ;
  egammaMonitoring::EffRecPlots Eff_Reco;
  
  egammaMonitoring::ShowerShapesPlots ssp      ;
  egammaMonitoring::ShowerShapesPlots ssp_fe   ;
  egammaMonitoring::ShowerShapesPlots ssp_10cut;

  
  egammaMonitoring::ElectronPlots ep_a;
  egammaMonitoring::ElectronPlots ep_L;
  egammaMonitoring::ElectronPlots ep_M;
  egammaMonitoring::ElectronPlots ep_T;
  
  egammaMonitoring::PhotonPlots pp_a;

  egammaMonitoring::PhotonPlots pp_truthConvPhoton        ;
  egammaMonitoring::PhotonPlots pp_truthConvRecoConv      ;
  egammaMonitoring::PhotonPlots pp_truthConvRecoConv1Si   ;
  egammaMonitoring::PhotonPlots pp_truthConvRecoConv1TRT  ;
  egammaMonitoring::PhotonPlots pp_truthConvRecoConv2Si   ;
  egammaMonitoring::PhotonPlots pp_truthConvRecoConv2TRT  ;
  egammaMonitoring::PhotonPlots pp_truthConvRecoConv2SiTRT;
  egammaMonitoring::PhotonPlots pp_truthConvRecoUnconv    ;
  
  egammaMonitoring::PhotonPlots pp_truthUnconvPhoton        ;
  egammaMonitoring::PhotonPlots pp_truthUnconvRecoConv      ;
  egammaMonitoring::PhotonPlots pp_truthUnconvRecoConv1Si   ;
  egammaMonitoring::PhotonPlots pp_truthUnconvRecoConv1TRT  ;
  egammaMonitoring::PhotonPlots pp_truthUnconvRecoConv2Si   ;
  egammaMonitoring::PhotonPlots pp_truthUnconvRecoConv2TRT  ;
  egammaMonitoring::PhotonPlots pp_truthUnconvRecoConv2SiTRT;
  egammaMonitoring::PhotonPlots pp_truthUnconvRecoUnconv    ;

  // Histos
  // General Info
  TH1D *evtNmb = nullptr; //!
  
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
