/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFInvMassHypoTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaMonitoring/Monitored.h"

class ISvcLocator;
TrigMuonEFInvMassHypoTool::TrigMuonEFInvMassHypoTool(const std::string & type, const std::string & name, const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)){
}

TrigMuonEFInvMassHypoTool::~TrigMuonEFInvMassHypoTool(){
}

StatusCode TrigMuonEFInvMassHypoTool::initialize(){
  if(m_acceptAll) {
    ATH_MSG_INFO("Accepting all the events!");
  } else {
    if(m_invMassLow<0 && m_invMassHigh<0){ 
      ATH_MSG_ERROR("Both mass cuts are <0. This is probably a configuration mistake.");
      return StatusCode::FAILURE;
    }
  }
  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("MonTool name: " << m_monTool);
  }
  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

bool TrigMuonEFInvMassHypoTool::decideOnMassPair(TrigMuonEFInvMassHypoTool::MuonEFInfo& input) const{
  ATH_MSG_DEBUG( "deciding...");
  //Monitored Variables
  std::vector<float> fexInvMass;
  auto muonMassMon = Monitored::Collection("Pt", fexInvMass);
  auto monitorIt	= Monitored::Group(m_monTool, muonMassMon); 
  bool result = false;
  bool passLow = false;
  bool passHigh = false;
  //for pass through mode
  if(m_acceptAll) {
    result = true;
    ATH_MSG_DEBUG("Accept property is set: taking all the events");
    return result;
  }

  //  decision making
  //Get xAOD::MuonContainer from hypotool
  const xAOD::Muon* muon1 = input.muon1;
  const xAOD::Muon* muon2 = input.muon2;
  if( !muon1 || !muon2 ){
    ATH_MSG_DEBUG("Retrieval of xAOD::MuonContainer failed");
    return false;
  }
  if (muon1->primaryTrackParticle() && muon2->primaryTrackParticle()) { // was there a muon in this RoI ?
    const xAOD::TrackParticle* tr1 = muon1->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
    const xAOD::TrackParticle* tr2 = muon2->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
    if (!tr1 || !tr2) {
      ATH_MSG_DEBUG("No CombinedTrackParticle found.");
    } else {
      ATH_MSG_DEBUG("Retrieved CombinedTrack tracks with abs pt "<< (*tr1).pt()/CLHEP::GeV << " GeV and "<< (*tr2).pt()/CLHEP::GeV << " GeV ");

      float diMuMass = (tr1->p4()+tr2->p4()).M()/CLHEP::GeV;
      //fill monitored variables
      fexInvMass.push_back(diMuMass);
      //Apply hypo cuts
      if(m_invMassLow>0 && diMuMass>m_invMassLow) passLow=true;
      if(m_invMassHigh>0 && diMuMass<m_invMassHigh) passHigh = true;

      if(passLow && passHigh) result=true;

      ATH_MSG_DEBUG(" REGTEST diMuon mass is " << diMuMass << " GeV "
      	      << " and lowMassCut cut is " << m_invMassLow << " GeV"
      	      << " and highMassCut cut is " << m_invMassHigh << " GeV"
      	      << " so hypothesis is " << (result?"true":"false"));
    }
  }
  return result;	
}
  
StatusCode TrigMuonEFInvMassHypoTool::decide(std::vector<MuonEFInfo>& toolInput) const {

  for (auto& tool : toolInput){
    if(TrigCompositeUtils::passed(m_decisionId.numeric(), tool.previousDecisionIDs1) && TrigCompositeUtils::passed(m_decisionId.numeric(), tool.previousDecisionIDs2)){
      if(decideOnMassPair(tool)==true){
	ATH_MSG_DEBUG("Passes selection");
	TrigCompositeUtils::addDecisionID(m_decisionId, tool.decision);
      }
      else ATH_MSG_DEBUG("Does not pass selection");
    }
  }

  return StatusCode::SUCCESS;
}
