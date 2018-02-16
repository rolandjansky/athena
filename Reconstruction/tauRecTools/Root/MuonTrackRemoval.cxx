/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  , m_thMuonCalibrationTool("")
  , m_thMuonSelectionTool("")
{
  declareProperty("MuonCalibrationToolHandle", m_thMuonCalibrationTool);
  declareProperty("MuonSelectionToolHandle", m_thMuonSelectionTool);
  declareProperty("muonIDWP", m_iMaxMuonIDWP = 2); // tight 0, medium 1, loose 2, veryloose 3
}

//________________________________________
MuonTrackRemoval::~MuonTrackRemoval(){}


//________________________________________
StatusCode MuonTrackRemoval::initialize(){
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

  xAOD::Muon* mu = nullptr;
  for(auto xMuon : *muons){
    TLorentzVector muonp4 = xMuon->p4();
    if(taup4.DeltaR(muonp4) < 0.4){
      mu = nullptr;
      if( !(*m_thMuonCalibrationTool).correctedCopy( *xMuon, mu ) ) {
	ATH_MSG_ERROR( "MuonCalibrationTool can not really apply calibration nor smearing" );
	continue;
      }
      xAOD::Muon::Quality quality = (*m_thMuonSelectionTool).getQuality(*mu);
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

  static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::TrackParticleContainer>>> acc_InDetTrackParticles("trackLinks");
  
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
