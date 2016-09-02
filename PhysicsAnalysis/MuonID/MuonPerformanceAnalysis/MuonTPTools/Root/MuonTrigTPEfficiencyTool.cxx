/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTrigTPEfficiencyTool.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc / a.m.
 */

#include "MuonTPTools/MuonTrigTPEfficiencyTool.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"

MuonTrigTPEfficiencyTool::MuonTrigTPEfficiencyTool(std::string myname)
: MuonTPEfficiencyTool(myname)
{
	declareProperty("dR_L1",     m_dR_L1 = 0.2);
	declareProperty("dR_HLT",    m_dR_HLT= 0.25);
}



//---------------------------------------------------------
void MuonTrigTPEfficiencyTool::matchProbes(ProbeContainer* probes, const xAOD::IParticleContainer* ) const
{
	for(auto probe : *probes) {
		double best_dR_L1  = m_dR_L1;
		double best_dR_HLT = m_dR_HLT;

		const xAOD::Muon* matchmuon = dynamic_cast<const xAOD::Muon*>(&(probe->probeTrack()));
		if (!matchmuon) {
			ATH_MSG_WARNING("Running trigger efficiency on something that is not a muon!");
			continue;
		}

		// Pt Cut
		if(matchmuon->pt()        < m_matchPtCut) continue;
		// Eta Cut
		if(fabs(matchmuon->eta()) > m_matchEtaCut) continue;

		probe->SetCurrentTrigger(m_trigger_item);
		probe->sf_trig(1.);
		probe->dr_match(-1.);
		// L1 trigger matching
		if ( m_trigger_item.find("L1_")==0 && m_trigger_item != "L1_AllTiggers") {
			bool trigmatchL1 = false;
			try{
				if(m_rerunMode)
					trigmatchL1 = m_trigUtils->Trig_Match_RM(*matchmuon,m_trigger_item,m_maximumDrCut);
				else
					trigmatchL1 = m_trigUtils->Trig_Match(*matchmuon, m_trigger_item, m_maximumDrCut);
			}
			catch(SG::ExcNoAuxStore){
				ATH_MSG_INFO("Unable to read trigger info for L1 matching to "<<m_trigger_item);
				trigmatchL1 = false;
			}
			probe->isMatched(trigmatchL1);
			best_dR_L1 = m_trigUtils->Trig_DR(*matchmuon,m_trigger_item);
			probe->dr_match(best_dR_L1);
			if (m_do_sf) {
				double sf = 1.;
				if (getTrigSF(matchmuon, sf) == CP::CorrectionCode::Ok){
					probe->sf_trig(sf);
				}
				else {
					if (m_do_sf) ATH_MSG_DEBUG("Could not apply SF for "<<m_trigger_item);
					probe->sf_trig(1.);
				}
			}
			if(!trigmatchL1)  continue;
			probe->HasSomeTrigger(true);
			probe->dRL1  = best_dR_L1;
		}
		if (m_trigger_item == "L1_AllTriggers"){
			probe->isMatched(probe->HasSomeTrigger());
		}

		// HLT trigger matching
		if ( m_trigger_item.find("HLT_")==0 && m_trigger_item != "HLT_AllTriggers") {
			bool match_HLT_trig = false;
			try{
				if(m_rerunMode)
					match_HLT_trig = m_trigUtils->Trig_Match_RM(*matchmuon,m_trigger_item,m_maximumDrCut);
				else
					match_HLT_trig = m_trigUtils->Trig_Match(*matchmuon,m_trigger_item,m_maximumDrCut);
			}
			catch(SG::ExcNoAuxStore){
				ATH_MSG_INFO("Unable to read trigger info for HLT matching to "<<m_trigger_item);
				match_HLT_trig = false;
			}
			probe->isMatched(match_HLT_trig);
			best_dR_HLT = m_trigUtils->Trig_DR(*matchmuon,m_trigger_item);
			probe->dr_match(best_dR_HLT);
			if (m_do_sf) {
				double sf = 1.;
				if (getTrigSF(matchmuon, sf) == CP::CorrectionCode::Ok){
					probe->sf_trig(sf);
				}
				else{
					if (m_do_sf) ATH_MSG_DEBUG("Could not apply SF for "<<m_trigger_item);
					probe->sf_trig(1.);
				}
			}
			if (!match_HLT_trig) continue;
			probe->HasSomeTrigger_HLT(true);
			probe->dRHLT = best_dR_HLT;
		}
		if (m_trigger_item == "HLT_AllTriggers") {
			probe->isMatched(probe->HasSomeTrigger_HLT());
		}
	}

	if (msgLvl(MSG::DEBUG)) {
		int nmatched=0;
		for(auto probe : *probes)
			if(probe->isMatched())
				nmatched++;
		ATH_MSG_DEBUG("Number of matched probes    : " << nmatched );
	}

	///  StatusCode sc =  m_trigDec.retrieve();
}

CP::CorrectionCode MuonTrigTPEfficiencyTool::getTrigSF(const xAOD::Muon * probe, double & sf) const
{    
	xAOD::MuonContainer* cont = new xAOD::MuonContainer();
	xAOD::MuonAuxContainer* aux = new xAOD::MuonAuxContainer();
	cont->setStore(aux);
	xAOD::Muon* copy = new xAOD::Muon();
	copy->makePrivateStore(probe);
	cont->push_back(copy);
	m_trig_sf_tool->setRunNumber(267638);
	CP::CorrectionCode outcome = m_trig_sf_tool->getTriggerScaleFactor(*cont,
			sf,
			m_trigger_item);
	delete cont;
	delete aux;
	return outcome;
}

