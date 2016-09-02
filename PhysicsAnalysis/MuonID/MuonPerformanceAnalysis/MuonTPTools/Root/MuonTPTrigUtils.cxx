/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPTrigUtils.cxx
 *
 *  Created on: Oct 9, 2015
 *      Author: goblirsc
 */

#include "MuonTPTools/MuonTPTrigUtils.h"
#include "xAODEventInfo/EventInfo.h"

MuonTPTrigUtils::MuonTPTrigUtils(std::string myname):
    AsgTool(myname),
    m_trigTool("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::ITrigMuonMatching/TrigMuonMatching")  {
    declareProperty("TriggerDecisionTool",     m_trigTool);
    declareProperty("TriggerMatchTool",        m_matchTool);

}

StatusCode MuonTPTrigUtils::initialize() {
	ATH_CHECK(m_trigTool.retrieve());
    ATH_CHECK(m_matchTool.retrieve());
    return StatusCode::SUCCESS;
}

bool MuonTPTrigUtils::TrigDecision(const std::string& trig) {

    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }
	SG::AuxElement::Decorator< bool > decorated (std::string("event_decorated_Trigger_"+trig).c_str());
	SG::AuxElement::Decorator< bool > dec_trig  (std::string("event_TrigDecision_"+trig).c_str());
	if (!decorated.isAvailable(*info) || !decorated(*info)){
		dec_trig(*info) = m_trigTool->isPassed(trig);
		decorated(*info) = true;
	}
	return dec_trig(*info);
}

bool MuonTPTrigUtils::TrigDecision_RM(const  std::string& trig) {

    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }
	SG::AuxElement::Decorator< bool > decorated (std::string("event_decorated_trig_RM_"+trig).c_str());
	SG::AuxElement::Decorator< bool > dec_trig  (std::string("event_TrigDecision_RM_"+trig).c_str());
	if (!decorated.isAvailable(*info) || !decorated(*info)){
		dec_trig(*info) = m_matchTool->isPassedRerun(trig);
		decorated(*info) = true;
	}
	return dec_trig(*info);
}

float MuonTPTrigUtils::Trig_DR(const xAOD::Muon & mu,const  std::string& trig, const float dr) {

	SG::AuxElement::Decorator< bool > decorated (std::string("decorated_trig_dR_"+trig+Form("%.2f",dr)).c_str());
	SG::AuxElement::Decorator< float > dec_dR  (std::string("trig_dR_"+trig).c_str());
	if (!decorated.isAvailable(mu) || !decorated(mu)){
		dec_dR(mu) = m_matchTool->minDelR(&mu, trig,dr);
		decorated(mu) = true;
	}
	return dec_dR(mu);
}

bool MuonTPTrigUtils::Trig_Match(const xAOD::Muon & mu,const  std::string& trig, const float  dr) {
		// L1 trigger
	if ( trig.find("L1_")==0 || trig.find("HLT_noalg_L1MU")==0 ) {
		return Trig_Match_L1(mu,trig,dr);
	}
	// HLT
	else {
		return Trig_Match_HLT(mu,trig,dr);
	}
}
bool MuonTPTrigUtils::Trig_Match_HLT (const xAOD::Muon & mu,const  std::string & trig, const float  dr){
	SG::AuxElement::Decorator< bool > decorated (std::string("decorated_trig_"+trig+Form("%.2f",dr)).c_str());
	SG::AuxElement::Decorator< bool > dec_trig  (std::string("trigmatch_"+trig+Form("%.2f",dr)).c_str());
	if (!decorated.isAvailable(mu) || !decorated(mu)){
		try{
			dec_trig(mu) = (TrigDecision(trig) && m_matchTool->match(&mu, trig,dr));
		}
		catch(SG::ExcNoAuxStore){
			ATH_MSG_WARNING("Unable to read trigger info for HLT matching to "<<trig);
			dec_trig(mu) = false;
		}
		decorated(mu) = true;
	}
	return dec_trig(mu);
}

bool MuonTPTrigUtils::Trig_Match_HLT_RM (const xAOD::Muon & mu,const  std::string & trig, const float  dr){
	SG::AuxElement::Decorator< bool > decorated (std::string("decorated_trig_RM_"+trig+Form("%.2f",dr)).c_str());
	SG::AuxElement::Decorator< bool > dec_trig  (std::string("trigmatch_RM_"+trig+Form("%.2f",dr)).c_str());
	if (!decorated.isAvailable(mu) || !decorated(mu)){
		try{
			dec_trig(mu) = (TrigDecision_RM(trig) && Trig_DR(mu, trig)<dr);
		}
		catch(SG::ExcNoAuxStore){
			ATH_MSG_WARNING("Unable to read trigger info for HLT matching to "<<trig);
			dec_trig(mu) = false;
		}
		decorated(mu) = true;
	}
	return dec_trig(mu);
}

bool MuonTPTrigUtils::Trig_Match_L1 (const xAOD::Muon & mu,const  std::string & trig, const float  dr){
	SG::AuxElement::Decorator< bool > decorated (std::string("decorated_trig_"+trig+Form("%.2f",dr)).c_str());
	SG::AuxElement::Decorator< bool > dec_trig  (std::string("trigmatch_"+trig+Form("%.2f",dr)).c_str());
	if (!decorated.isAvailable(mu) || !decorated(mu)){
		std::string mutrig=trig;
		if(trig.find("HLT_noalg_L1MU")==0)
			mutrig="L1_MU"+trig.substr(14);
		try{
			dec_trig(mu) = (TrigDecision(trig) && m_matchTool->matchL1(&mu, mutrig,dr));
		}
		catch(SG::ExcNoAuxStore){
			ATH_MSG_WARNING("Unable to read trigger info for L1 matching to "<<mutrig);
			dec_trig(mu) = false;
		}
		decorated(mu) = true;
	}
	return dec_trig(mu);
}


bool MuonTPTrigUtils::Trig_Match_RM(const xAOD::Muon & mu,const  std::string& trig, const float  dr) {
	// L1 trigger: Treat similar to normal L1?! (Taken from existing code)
	if ( trig.find("L1_")==0 || trig.find("HLT_noalg_L1MU")==0 ) {
		return Trig_Match_L1(mu,trig,dr);
	}
	// HLT
	else {
		return Trig_Match_HLT_RM(mu,trig, dr );
	}
}
