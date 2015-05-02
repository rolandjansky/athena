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

MuonTrigTPEfficiencyTool::MuonTrigTPEfficiencyTool(std::string myname)
: AsgTool(myname), MuonTPEfficiencyTool(myname)
{
    declareProperty("dR_L1",           m_dR_L1 = 0.2);
    declareProperty("dR_HLT",          m_dR_HLT= 0.25);
    declareProperty("TrigItem",        m_trigger_item="");
}



//---------------------------------------------------------
void MuonTrigTPEfficiencyTool::matchProbes(ProbeContainer* probes, const xAOD::IParticleContainer* ) const
{
    for(auto probe : *probes)
    {
        double best_dR_L1  = m_dR_L1;
        double best_dR_HLT = m_dR_HLT;
        
        const xAOD::Muon* matchmuon = dynamic_cast<const xAOD::Muon*>(&(probe->probeTrack()));
        if (!matchmuon)
	{
            ATH_MSG_WARNING("Running trigger efficiency on something that is not a muon!");
            continue;
        }
        
	// Pt Cut
	if(matchmuon->pt()        < m_matchPtCut) continue;
        // Eta Cut
        if(fabs(matchmuon->eta()) > m_matchEtaCut) continue;
        
	probe->SetCurrentTrigger(m_trigger_item);
	
	// L1 trigger matching
	if ( m_trigger_item.find("L1_")!=std::string::npos && m_trigger_item != "L1_AllTriggers")
	{
            bool trigmatchL1 = m_matchTool->matchL1(matchmuon,m_trigger_item, m_dR_L1);
            probe->isMatched(trigmatchL1);
            if(!trigmatchL1)  continue;
            probe->HasSomeTrigger(true);
            best_dR_L1   = m_matchTool->minDelRL1(matchmuon,m_trigger_item);
            probe->dRL1  = best_dR_L1;
        }
        if (m_trigger_item == "L1_AllTriggers")
	{
            probe->isMatched(probe->HasSomeTrigger());
        }

	// HLT trigger matching
	if ( m_trigger_item.find("HLT_")!=std::string::npos && m_trigger_item != "HLT_AllTriggers")
	{
            bool match_HLT_trig = m_matchTool->match(matchmuon->eta(), matchmuon->phi(), m_trigger_item);
            probe->isMatched(match_HLT_trig);
            if (!match_HLT_trig) continue;
            probe->HasSomeTrigger_HLT(true);
            best_dR_HLT  = m_matchTool->minDelR(matchmuon,m_trigger_item);
	    probe->dRHLT = best_dR_HLT;
        }
	if (m_trigger_item == "HLT_AllTriggers")
	{
            probe->isMatched(probe->HasSomeTrigger_HLT());
        }
    }

    if (msgLvl(MSG::DEBUG))
    {
        int nmatched=0;
        for(auto probe : *probes) 
            if(probe->isMatched())
	        nmatched++;    
        ATH_MSG_DEBUG("Number of matched probes    : " << nmatched );
    }
    
///  StatusCode sc =  m_trigDec.retrieve();
}

