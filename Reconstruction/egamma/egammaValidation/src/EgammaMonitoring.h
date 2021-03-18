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
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "PATCore/AcceptData.h"


#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "TruthElectronHistograms.h"
#include "TruthPhotonHistograms.h"
#include "RecoElectronHistograms.h"
#include "RecoPhotonHistograms.h"
#include "IHistograms.h"
#include "ShowerShapesHistograms.h"
#include "ClusterHistograms.h"
#include "TrackHistograms.h"
#include "EfficiencyPlot.h"
#include "WidthPlot.h"

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
  ITHistSvc*   rootHistSvc = nullptr;


  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterAll;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> cluster10GeV;
 
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterPromptAll;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterPrompt10GeV;

  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterConvPhoton;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterConvPhotonSi;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterConvPhotonSiSi;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterConvPhotonTRT;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterConvPhotonTRTTRT;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterConvPhotonSiTRT;
  std::unique_ptr<egammaMonitoring::ClusterHistograms> clusterUnconvPhoton;


  std::unique_ptr<egammaMonitoring::ShowerShapesHistograms> showerShapesAll;
  std::unique_ptr<egammaMonitoring::ShowerShapesHistograms> showerShapes10GeV;
  std::unique_ptr<egammaMonitoring::IsolationHistograms> isolationAll;

  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthElectronAll;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthPromptElectronAll;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthElectronRecoElectronAll;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthPromptElectronWithTrack;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthPromptElectronWithGSFTrack;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthPromptElectronWithReco;
  std::unique_ptr<egammaMonitoring::IHistograms> recoElectronAll;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthRecoElectronLooseLH;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthRecoElectronMediumLH;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> truthRecoElectronTightLH;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> recoElectronIsoFixedCutTight;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> recoElectronIsoFixedCutTightTrackOnly;
  std::unique_ptr<egammaMonitoring::TruthElectronHistograms> recoElectronIsoFixedCutLoose;

  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonAll;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonAll;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonAllUnconv;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonAllConv;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonRecoPhoton        ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonRecoPhotonOrElectron;
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
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoConv1Si   ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoConv1TRT  ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoConv2Si   ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoConv2TRT  ;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoConv2SiTRT;
  std::unique_ptr<egammaMonitoring::IHistograms> truthPhotonUnconvRecoUnconv    ;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonUnconvIsoFixedCutTight;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonUnconvIsoFixedCutTightCaloOnly;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonUnconvIsoFixedCutLoose;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonConvIsoFixedCutTight;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonConvIsoFixedCutTightCaloOnly;
  std::unique_ptr<egammaMonitoring::IHistograms> recoPhotonConvIsoFixedCutLoose;

  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracks;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksMatchElectron;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksNotElectron;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksMatchPion;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksNotMatched;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTMatchElectron;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTNotElectron;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTMatchPion;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTNotMatched;

  std::unique_ptr<egammaMonitoring::IHistograms> InDetTrackshighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksMatchElectronhighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksNotElectronhighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksMatchPionhighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksNotMatchedhighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRThighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTMatchElectronhighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTNotElectronhighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTMatchPionhighpT;
  std::unique_ptr<egammaMonitoring::IHistograms> InDetTracksTRTNotMatchedhighpT;


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

  bool matchedToElectron(const xAOD::TrackParticle& tp);
  bool matchedToPion(const xAOD::TrackParticle& tp);
  bool notMatchedToTruth(const xAOD::TrackParticle& tp);

};

#endif
