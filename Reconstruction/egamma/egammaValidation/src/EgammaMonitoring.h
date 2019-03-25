/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
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

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/xAODTruthHelpers.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"


#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "TruthElectronHistograms.h"
#include "TruthPhotonHistograms.h"
#include "RecoElectronHistograms.h"
#include "RecoPhotonHistograms.h"
#include "IHistograms.h"
#include "ShowerShapesHistograms.h"
#include "ClusterHistograms.h"
#include "EfficiencyPlot.h"

#include "IsolationHistograms.h"
#include "IsolationSelection/IIsolationSelectionTool.h"

#include "TFile.h"
#include "TH1.h"

#include <string>
#include <vector>
#include <cmath>

class EgammaMonitoring : public AthAlgorithm
{
 public:

  /// Tools and services ///
  ITHistSvc*   rootHistSvc ;

//  egammaMonitoring::EffIDPlots  Eff_ID  ;
//  egammaMonitoring::EffRecPlots Eff_Reco;




  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterAll;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> cluster10GeV;
 
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterPromptAll;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterPrompt10GeV;


  std::unique_ptr<egammaMonitoring::ShowerShapesHistograms> showerShapesAll;
  std::unique_ptr<egammaMonitoring::ShowerShapesHistograms> showerShapes10GeV;
  std::unique_ptr<egammaMonitoring::IsolationHistograms> isolationAll;

  std::unique_ptr<egammaMonitoring::IHistograms> truthElectronAll;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPromptElectronAll;
  std::unique_ptr<egammaMonitoring::IHistograms> truthElectronRecoElectronAll;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPromptElectronWithTrack;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPromptElectronWithGSFTrack;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPromptElectronWithReco;
  std::unique_ptr<egammaMonitoring::IHistograms> recoElectronAll;
  std::unique_ptr<egammaMonitoring::IHistograms> truthRecoElectronLooseLH;
  std::unique_ptr<egammaMonitoring::IHistograms> truthRecoElectronMediumLH;
  std::unique_ptr<egammaMonitoring::IHistograms> truthRecoElectronTightLH;
  std::unique_ptr<egammaMonitoring::IHistograms> recoElectronIsoFixedCutTight;
  std::unique_ptr<egammaMonitoring::IHistograms> recoElectronIsoFixedCutTightTrackOnly;
  std::unique_ptr<egammaMonitoring::IHistograms> recoElectronIsoFixedCutLoose;

  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonAll;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonRecoPhoton        ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvPhoton        ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvRecoConv      ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvRecoConv1Si   ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvRecoConv1TRT  ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvRecoConv2Si   ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvRecoConv2TRT  ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvRecoConv2SiTRT;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonConvRecoUnconv    ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvPhoton        ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoConv      ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoUnconv    ;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonUnconvIsoFixedCutTight;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonUnconvIsoFixedCutTightCaloOnly;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonUnconvIsoFixedCutLoose;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonConvIsoFixedCutTight;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonConvIsoFixedCutTightCaloOnly;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonConvIsoFixedCutLoose;

  // Histos
  // General Info
  TH1D *evtNmb = nullptr; //!

  EgammaMonitoring (const std::string& name, ISvcLocator* pSvcLocator);
  ~EgammaMonitoring(){};


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
  asg::AnaToolHandle<CP::IIsolationSelectionTool> m_IsoFixedCutTight; //!
  asg::AnaToolHandle<CP::IIsolationSelectionTool> m_IsoFixedCutTightTrackOnly; //!
  asg::AnaToolHandle<CP::IIsolationSelectionTool> m_IsoFixedCutTightCaloOnly; //!
  asg::AnaToolHandle<CP::IIsolationSelectionTool> m_IsoFixedCutLoose; //!

  ToolHandle<IMCTruthClassifier>  m_mcTruthClassifier;

};

#endif
