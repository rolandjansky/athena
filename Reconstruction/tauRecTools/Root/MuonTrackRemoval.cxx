/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/MuonTrackRemoval.h"
#include "xAODMuon/MuonContainer.h"

#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

using namespace tauRecTools;

//________________________________________
MuonTrackRemoval::MuonTrackRemoval(const std::string& name):
  TauRecToolBase(name)  
{
  declareProperty("muonIDWP", m_iMaxMuonIDWP = 2); // tight 0, medium 1, loose 2, veryloose 3 
}

//________________________________________
MuonTrackRemoval::~MuonTrackRemoval(){}


//________________________________________
StatusCode MuonTrackRemoval::initialize(){

  ANA_CHECK(m_thMuonCalibrationTool = new asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool>( "MuonCorrectionTool" ));
  ATH_CHECK(ASG_MAKE_ANA_TOOL(*m_thMuonCalibrationTool, CP::MuonCalibrationAndSmearingTool));
  ANA_CHECK(m_thMuonCalibrationTool->setProperty( "Year", "Data16" ));
  ANA_CHECK(m_thMuonCalibrationTool->setProperty("Release","Recs2016_15_07"));
  ANA_CHECK(m_thMuonCalibrationTool->setProperty("StatComb",false));
  ANA_CHECK(m_thMuonCalibrationTool->setProperty("SagittaCorr",true));
  ANA_CHECK(m_thMuonCalibrationTool->setProperty("SagittaRelease", "sagittaBiasDataAll_02_08_17"));
  ANA_CHECK(m_thMuonCalibrationTool->setProperty("doSagittaMCDistortion",false));
  ANA_CHECK(m_thMuonCalibrationTool->initialize());
  
  m_thMuonSelectionTool = new asg::AnaToolHandle<CP::IMuonSelectionTool>("MuonSelectionTool");
  ATH_CHECK(ASG_MAKE_ANA_TOOL(*m_thMuonSelectionTool, CP::MuonSelectionTool));
  ANA_CHECK(m_thMuonSelectionTool->setProperty("MuQuality", 3));
  ANA_CHECK(m_thMuonSelectionTool->initialize());

  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode MuonTrackRemoval::finalize(){
  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode MuonTrackRemoval::execute(xAOD::TauJet& xTau){
  const xAOD::MuonContainer* muons = evtStore()->retrieve<const xAOD::MuonContainer>("Muons");  
  std::vector<ElementLink <xAOD::TrackParticleContainer>> muTracks;
  TLorentzVector taup4 = xTau.p4();

  xAOD::Muon* mu = 0;
  for(auto xMuon : *muons){
    TLorentzVector muonp4 = xMuon->p4();
    if(taup4.DeltaR(muonp4) < 0.4){
      mu = 0;
      if( !(*m_thMuonCalibrationTool)->correctedCopy( *xMuon, mu ) ) {
	Error( "Tes", "Cannot really apply calibration nor smearing" );
	continue;
      }
      xAOD::Muon::Quality quality = (*m_thMuonSelectionTool)->getQuality(*mu);
      if(quality <= m_iMaxMuonIDWP){
        const ElementLink< xAOD::TrackParticleContainer > muIDTrack = mu->inDetTrackParticleLink();
	if(muIDTrack.isValid())
	  muTracks.push_back(muIDTrack);
      }
      delete mu;
    }
  }
  if(muTracks.size() == 0)
    return StatusCode::SUCCESS;

  static SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::TrackParticleContainer>>> acc_InDetTrackParticles("trackLinks");
  
  std::vector< ElementLink<xAOD::TauTrackContainer>> tauTrackLinks = xTau.allTauTrackLinks();
  std::vector< ElementLink<xAOD::TauTrackContainer>> newTauTrackLinks;
  
  for(auto tautracklink : tauTrackLinks){
    bool muon_track = false;
    if(!tautracklink.isValid())
      continue;
    std::vector<ElementLink<xAOD::TrackParticleContainer>> tauInDetTrackParticleLinks = acc_InDetTrackParticles(**tautracklink);
    for(auto inDetTrackParticleLink : tauInDetTrackParticleLinks){
      if(!inDetTrackParticleLink.isValid())
        continue;
      for(auto mutrack : muTracks){
        if((*inDetTrackParticleLink) == (*mutrack)){
          muon_track = true;
          break;
        }
      }
      if(muon_track)
        break;
    }
    if(!muon_track)
      newTauTrackLinks.push_back(tautracklink);
  }
  xTau.setAllTauTrackLinks(newTauTrackLinks);
  return StatusCode::SUCCESS;
}
