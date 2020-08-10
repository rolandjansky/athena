/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFInvMassHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODMuon/MuonContainer.h"

TrigMuonEFInvMassHypoTool::TrigMuonEFInvMassHypoTool(const std::string & type, const std::string & name, const IInterface* parent):
  ComboHypoToolBase(type, name, parent)
{ }

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

bool TrigMuonEFInvMassHypoTool::executeAlg(std::vector<LegDecision> &combination) const{

  //Monitored Variables
  std::vector<float> fexInvMass, fexInvMassSel;
  auto muonMassMon = Monitored::Collection("Mass", fexInvMass);
  auto muonMassSelMon = Monitored::Collection("Mass_sel", fexInvMassSel);
  auto monitorIt	= Monitored::Group(m_monTool, muonMassMon, muonMassSelMon); 

  bool result = false;
  bool passLow = false;
  bool passHigh = false;

  //for pass through mode
  if(m_acceptAll) {
    result = true;
    ATH_MSG_DEBUG("Accept property is set: taking all the events");
    return result;
  }

  std::vector<const xAOD::Muon*> selected_muons;
  for(auto el: combination){
    auto EL= el.second;    
    auto dec= (*EL);
    auto muonLinks = TrigCompositeUtils::findLinks<xAOD::MuonContainer>( dec, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
    for(size_t i=0; i<muonLinks.size(); i++){
      if(muonLinks.at(i).isValid()){ 
	const xAOD::Muon *mu = *(muonLinks.at(i).link);
	if(mu->primaryTrackParticle()) selected_muons.push_back(mu);
      }
    }
  }


  //  decision making
  // Loop over muons passing previous hypo
  for(size_t i=0; i<selected_muons.size(); i++){
    const xAOD::Muon* muon1 = selected_muons[i];

    for(size_t j=i+1; j<selected_muons.size(); j++){
      const xAOD::Muon* muon2 = selected_muons[j];

      //We always get muons from previous decision that have combined track particles
      const xAOD::TrackParticle* tr1 = muon1->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
      const xAOD::TrackParticle* tr2 = muon2->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
      if (!tr1 || !tr2) {
	ATH_MSG_DEBUG("No CombinedTrackParticle found.");
      } else {
	ATH_MSG_DEBUG("Retrieved CombinedTrack tracks with abs pt "<< (*tr1).pt()/Gaudi::Units::GeV << " GeV and "<< (*tr2).pt()/Gaudi::Units::GeV << " GeV ");

	float diMuMass = (tr1->p4()+tr2->p4()).M()/Gaudi::Units::GeV;

	//fill monitored variables
	fexInvMass.push_back(diMuMass);

	ATH_MSG_DEBUG(" REGTEST diMuon mass is " << diMuMass << " GeV "
		      << " and lowMassCut cut is " << m_invMassLow << " GeV"
		      << " and highMassCut cut is " << m_invMassHigh << " GeV");

	//Apply hypo cuts. If any mass combination is true, then the overall result should be true.
	if(m_invMassLow>0 && diMuMass>m_invMassLow) passLow=true;
	if(m_invMassHigh>0 && diMuMass<m_invMassHigh) passHigh = true;
	if(passLow && passHigh){
	  result=true;
	  //fill monitored variables
	  fexInvMassSel.push_back(diMuMass);
	}
      }
    }
  }
  
  ATH_MSG_DEBUG("Overall result is: "<<(result?"true":"false"));
  return result;	
}
  
