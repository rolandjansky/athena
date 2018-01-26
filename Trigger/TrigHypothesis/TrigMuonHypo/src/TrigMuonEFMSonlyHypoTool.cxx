/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFMSonlyHypoTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/MonitoredScope.h"

class ISvcLocator;

TrigMuonEFMSonlyHypoTool::TrigMuonEFMSonlyHypoTool(const std::string & type, const std::string & name, const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)){
  declareProperty("AcceptAll", m_acceptAll=true);
  std::vector<float> def_bins;
  def_bins.push_back(0);
  def_bins.push_back(9.9);
  std::vector<float> def_thrs;
  def_thrs.push_back(0.*CLHEP::GeV);
  declareProperty("PtBins", m_ptBins=def_bins);
  declareProperty("PtThresholds", m_ptThresholds=def_thrs);

  m_bins = 0;

}

TrigMuonEFMSonlyHypoTool::~TrigMuonEFMSonlyHypoTool(){
}

StatusCode TrigMuonEFMSonlyHypoTool::initialize(){


  if(m_acceptAll) {
    ATH_MSG_INFO("Accepting all the events with not cut!");
  } else {
    m_bins = m_ptBins.size() - 1;
    if (m_bins != m_ptThresholds.size()) {
      ATH_MSG_ERROR("bad thresholds setup .... exiting!");
      return StatusCode::FAILURE;
    }

    for (std::vector<float>::size_type i=0; i<m_bins;++i) {
      ATH_MSG_INFO( "bin " << m_ptBins[i] << " - " <<  m_ptBins[i+1]<<" with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV<< " GeV");
    }
  }

  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}


StatusCode TrigMuonEFMSonlyHypoTool::decide(TrigMuonEFMSonlyHypoTool::MuonEFInfo& input) const{


  ATH_MSG_DEBUG( "deciding...");

  using namespace Monitored;
  //Monitored Variables
  std::vector<float> fexPt, fexEta, fexPhi;

  auto muonPtMon = MonitoredCollection::declare("MSonlyTrkPt", fexPt);
  auto muonEtaMon = MonitoredCollection::declare("MSonlyTrkEta", fexEta);
  auto muonPhiMon = MonitoredCollection::declare("MSonlyTrkPhi", fexPhi);

  bool result = false;
  //for pass through mode
  if(m_acceptAll) {
    result = true;
    ATH_MSG_DEBUG("Accept property is set: taking all the events");
    return StatusCode(result);
  }

  //  decision making
  //Get xAOD::MuonContainer from hypotool
  const xAOD::MuonContainer *muonContainer = input.muons;
  if(!muonContainer){
    ATH_MSG_DEBUG("Retrieval of xAOD::MuonContainer failed");
    return StatusCode::FAILURE;
  }

  for(auto muon : *muonContainer){
    if (muon->primaryTrackParticle()) { // was there a muon in this RoI ?

      const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);

      if (!tr) {
	ATH_MSG_DEBUG("No ExtrapolatedMuonSpectrometerTrackParticle found.");
	continue;
      } else {
	ATH_MSG_DEBUG("Retrieved ExtrapolatedMuonSpectrometerTrack track with abs pt "<< (*tr).pt()/CLHEP::GeV << " GeV ");

	//fill monitored variables
	fexPt.push_back(tr->pt()/CLHEP::GeV);
	fexEta.push_back(tr->eta());
	fexPhi.push_back(tr->phi());

	//Apply hypo cuts
	float absEta = fabs(tr->eta());
	float threshold = 0;
	for (std::vector<float>::size_type k=0; k<m_bins; ++k) {
	  if (absEta > m_ptBins[k] && absEta <= m_ptBins[k+1]) threshold = m_ptThresholds[k];
	}
	if (fabs(tr->pt())/CLHEP::GeV > (threshold/CLHEP::GeV)){
	  result = true;
	}
	ATH_MSG_DEBUG(" REGTEST muon pt is " << tr->pt()/CLHEP::GeV << " GeV "
		      << " with Charge " << tr->charge()
		      << " and threshold cut is " << threshold/CLHEP::GeV << " GeV"
		      << " so hypothesis is " << (result?"true":"false"));
      }
    }
  }
  return StatusCode(result);				
}
  

StatusCode TrigMuonEFMSonlyHypoTool::decide(std::vector<MuonEFInfo>& toolInput) const {
  ATH_MSG_DEBUG("Applying selection of single << " << m_decisionId.numeric());
  for ( auto& i: toolInput) {
    if (TrigCompositeUtils::passed(m_decisionId.numeric(), i.previousDecisionIDs)) {
      if ( decide(i) ) {
	ATH_MSG_DEBUG("hypothesis is true, so muon passed selection");
	TrigCompositeUtils::addDecisionID(m_decisionId, i.decision);
      } else {
	ATH_MSG_DEBUG("hypothesis is false, so no muons passed selection");
      }
    } 
  } 
  return StatusCode::SUCCESS;
}
