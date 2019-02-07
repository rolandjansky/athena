/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/Combinators.h"
#include "TrigMuonEFCombinerHypoTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/Monitored.h"
class ISvcLocator;
TrigMuonEFCombinerHypoTool::TrigMuonEFCombinerHypoTool(const std::string & type, const std::string & name, const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)){
}
TrigMuonEFCombinerHypoTool::~TrigMuonEFCombinerHypoTool(){
}
StatusCode TrigMuonEFCombinerHypoTool::initialize(){
  if(m_acceptAll) {
    ATH_MSG_INFO("Accepting all the events with not cut!");
  } else {
    if(m_ptBins.size()<=0){ 
      ATH_MSG_ERROR("Trying to configure hypo with no pT bins. This is probably a configuration mistake.");
      return StatusCode::FAILURE;
    }
    m_bins.resize(m_ptBins.size());
    for(size_t j=0; j<m_ptBins.size(); j++){
      m_bins[j] = m_ptBins[j].size() - 1;
      if (m_bins[j] != m_ptThresholds[j].size()) {
	ATH_MSG_ERROR("bad thresholds setup .... exiting!");
	return StatusCode::FAILURE;
      }
      for (std::vector<float>::size_type i=0; i<m_bins[j];++i) {
	ATH_MSG_INFO( "bin " << m_ptBins[j][i] << " - " <<  m_ptBins[j][i+1]<<" with Pt Threshold of " << (m_ptThresholds[j][i])/CLHEP::GeV<< " GeV");
      }
    }
  }
  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("MonTool name: " << m_monTool);
  }
  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}
bool TrigMuonEFCombinerHypoTool::decideOnSingleObject(TrigMuonEFCombinerHypoTool::MuonEFInfo& input, size_t cutIndex) const{
  ATH_MSG_DEBUG( "deciding...");
  //Monitored Variables
  std::vector<float> fexPt, fexEta, fexPhi, selPt, selEta, selPhi;
  auto muonPtMon = Monitored::Collection("Pt", fexPt);
  auto muonEtaMon = Monitored::Collection("Eta", fexEta);
  auto muonPhiMon = Monitored::Collection("Phi", fexPhi);
  auto muonPtSelMon = Monitored::Collection("Pt_sel", selPt);
  auto muonEtaSelMon = Monitored::Collection("Eta_sel", selEta);
  auto muonPhiSelMon = Monitored::Collection("Phi_sel", selPhi);
  auto monitorIt	= Monitored::Group(m_monTool, muonPtMon, muonEtaMon, muonPhiMon, muonPtSelMon, muonEtaSelMon, muonPhiSelMon); 
  bool result = false;
  //for pass through mode
  if(m_acceptAll) {
    result = true;
    ATH_MSG_DEBUG("Accept property is set: taking all the events");
    return result;
  }
  //  decision making
  //Get xAOD::MuonContainer from hypotool
  const xAOD::Muon* muon = input.muon;
  if( !muon ){
    ATH_MSG_DEBUG("Retrieval of xAOD::MuonContainer failed");
    return false;
  }
  if (muon->primaryTrackParticle()) { // was there a muon in this RoI ?
    const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
    if (!tr) {
      ATH_MSG_DEBUG("No CombinedTrackParticle found.");
    } else {
      ATH_MSG_DEBUG("Retrieved CombinedTrack track with abs pt "<< (*tr).pt()/CLHEP::GeV << " GeV ");
      //fill monitored variables
      fexPt.push_back(tr->pt()/CLHEP::GeV);
      fexEta.push_back(tr->eta());
      fexPhi.push_back(tr->phi());
      //Apply hypo cuts
      float absEta = fabs(tr->eta());
      float threshold = 0;
      for (std::vector<float>::size_type k=0; k<m_bins[0]; ++k) {
        if (absEta > m_ptBins[cutIndex][k] && absEta <= m_ptBins[cutIndex][k+1]) threshold = m_ptThresholds[cutIndex][k];
      }
      if (fabs(tr->pt())/CLHEP::GeV > (threshold/CLHEP::GeV)){
        selPt.push_back(tr->pt()/CLHEP::GeV);
        selEta.push_back(tr->eta());
        selPhi.push_back(tr->phi());
        result = true;
      }
      ATH_MSG_DEBUG(" REGTEST muon pt is " << tr->pt()/CLHEP::GeV << " GeV "
      	      << " with Charge " << tr->charge()
      	      << " and threshold cut is " << threshold/CLHEP::GeV << " GeV"
      	      << " so hypothesis is " << (result?"true":"false"));
    }
  }
  return result;	
}
  
StatusCode TrigMuonEFCombinerHypoTool::decide(std::vector<MuonEFInfo>& toolInput) const {
  size_t numTrigger = m_ptBins.size();
  size_t numMuon=toolInput.size();
  if(numTrigger==1){
    ATH_MSG_DEBUG("Applying selection of single << " << m_decisionId);
    return inclusiveSelection(toolInput);
  }
  else{
    if(numMuon <=1) ATH_MSG_DEBUG("Not applying selection "<<m_decisionId<< " because the number of muons is "<<numMuon);
    else{
      ATH_MSG_DEBUG("Applying selection of multiplicity "<< m_decisionId);
      return multiplicitySelection(toolInput);
    }
  }
  return StatusCode::SUCCESS;
}
StatusCode TrigMuonEFCombinerHypoTool::inclusiveSelection(std::vector<MuonEFInfo>& toolInput) const{
  for (auto& tool : toolInput){
    if(TrigCompositeUtils::passed(m_decisionId.numeric(), tool.previousDecisionIDs)){
      if(decideOnSingleObject(tool, 0)==true){
	ATH_MSG_DEBUG("Passes selection");
	TrigCompositeUtils::addDecisionID(m_decisionId, tool.decision);
      }
      else ATH_MSG_DEBUG("Does not pass selection");
    }
  }
  return StatusCode::SUCCESS;
}
StatusCode TrigMuonEFCombinerHypoTool::multiplicitySelection(std::vector<MuonEFInfo>& toolInput) const{
  HLT::Index2DVec passingSelection(toolInput.size());
  for(size_t cutIndex=0; cutIndex < m_ptBins.size(); ++cutIndex) {
    size_t elementIndex{0};
    for(auto& i : toolInput){
      if(TrigCompositeUtils::passed(m_decisionId.numeric(), i.previousDecisionIDs)){
	if(decideOnSingleObject(i, cutIndex)==true){
	  ATH_MSG_DEBUG("Passing selection "<<m_decisionId << " , Index["<<elementIndex<<"]");
	  passingSelection[cutIndex].push_back(elementIndex);
	}
	else ATH_MSG_DEBUG("Not passing selection "<<m_decisionId << " , Index["<<elementIndex<<"]");
      }
      else{
	ATH_MSG_DEBUG("No match for decisionId "<<m_decisionId);
      }
      elementIndex++;
    }
    //If nothing passes, then we should stop
    if(passingSelection[cutIndex].empty()){
      ATH_MSG_DEBUG("No muons passed the selection "<<cutIndex<<" rejecting...");
      return StatusCode::SUCCESS;
    }
  }
  std::set<size_t> passingIndices;
  if(m_decisionPerRoI==true){
    auto notFromSameRoI = [&](const HLT::Index1DVec& comb){
      std::set<const xAOD::Muon*> setOfMuons;
      for (auto index : comb){
	setOfMuons.insert(toolInput[index].muon);
      }
      return setOfMuons.size()==comb.size();
    };
  
    HLT::elementsInUniqueCombinations(passingSelection, passingIndices, notFromSameRoI);
  }
  else{
    HLT::elementsInUniqueCombinations(passingSelection, passingIndices);
  }
  if(passingIndices.empty()){
    ATH_MSG_DEBUG("No muons passed selection "<<m_decisionId);
    return StatusCode::SUCCESS;
  }
  for(auto i : passingIndices){
    ATH_MSG_DEBUG("Muon["<<i<<"] passes "<<m_decisionId<<" with pT = "<<toolInput[i].muon->pt() << "GeV");
    TrigCompositeUtils::addDecisionID(m_decisionId.numeric(), toolInput[i].decision);
  }
  return StatusCode::SUCCESS;
}
