/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HighPtSingleMuonSelectionTool.cxx
 *
 *  Created on: Jun 18, 2015
 *      Author: goblirsc
 */

#include "MuonTPTools/HighPtSingleMuonSelectionTool.h"

HighPtSingleMuonSelectionTool::HighPtSingleMuonSelectionTool(std::string myname)
:  MuonTPSelectionTool(myname)
{
    declareProperty("DoProbeIsolation",     m_probe_iso=false);

    declareProperty("ProbeD0Cut",        m_probe_d0 = -1.00);
    declareProperty("ProbeD0SignCut",    m_probe_d0Sign = -1.00);
    declareProperty("ProbeZ0Cut",        m_probe_z0 = -1.00);

    declareProperty("ProbeIDHitCut",     m_probe_ID_hits = false);

    declareProperty("ProbeIsolationTool", m_probe_isotool);
}

StatusCode HighPtSingleMuonSelectionTool::initialize()
{
    ATH_CHECK(MuonTPSelectionTool::initialize());

    if (m_probe_iso) {
        ATH_CHECK(m_probe_isotool.retrieve());
        ATH_CHECK(m_probe_isotool->setIParticleCutsFrom(xAOD::Type::Muon));
    }
    return StatusCode::SUCCESS;
}


//**********************************************************************

ProbeContainer* HighPtSingleMuonSelectionTool::selectProbes(const xAOD::MuonContainer* , const xAOD::IParticleContainer* probes) const
{
    // get event weight
    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }

    float evtWeight = 1.0;
    if (info->eventType(xAOD::EventInfo::IS_SIMULATION)){
        evtWeight = info->mcEventWeight();
    }

    // prepare probe selection, log processed events
    ProbeContainer* probeCont = new ProbeContainer();
    FillCutFlows("Processed",evtWeight);

    // check GRL
    if (!passGRL(info)) return probeCont;
    FillCutFlows("GRL",evtWeight);

    bool have_probe = false;

    // Event Trigger
	if (!Event_Trigger()) return probeCont;
	FillCutFlows("Trigger", evtWeight);

    FillCutFlows("TagCandidates",evtWeight);

    FillCutFlows("TagQuality",evtWeight);

    FillCutFlows("TagPt",evtWeight);

    FillCutFlows("TagEta",evtWeight);

    FillCutFlows("TagIP",evtWeight);

    FillCutFlows("TagIsolation",evtWeight);

    // for each selected tag, loop over probes
    for(auto probe : *probes) {

        FillCutFlows("ProbeCandidates",evtWeight);

        FillCutFlows("ProbeInvMass",evtWeight);

        // Cut on probe eta, pT
        if (!PassProbeKinematics(probe)) continue;
        FillCutFlows("ProbeKinematics",evtWeight);

        // ID hits
        const xAOD::Muon* probemu = dynamic_cast<const xAOD::Muon*> (probe);
		if (m_probe_ID_hits && probemu) {
			if (!m_selection_tool->passedIDCuts(*probemu)) continue;
		}
		if (!probemu && m_probe_ID_hits) {
			const xAOD::TrackParticle *trk = dynamic_cast<const xAOD::TrackParticle*>(probe);
			if (!m_selection_tool->passedIDCuts(*trk)) continue;
		}
		// reject CaloTags failing the Loose WP
		if (probemu && probemu->muonType() == xAOD::Muon::CaloTagged && m_selection_tool->getQuality(*probemu) > xAOD::Muon::Loose){
			continue;
		}
//	    if (!this->TagTriggerMatch(tag)) continue;
//	    FillCutFlows("TagTrigger", evtWeight);
//
//	    // Fill rerun triggerd decision bits
//	    this->TagTriggerMatch_RerunMode(tag);

		FillCutFlows("ProbeQuality", evtWeight);

        if (!PassProbeIsoCuts(probe)) continue;
        FillCutFlows("ProbeIsolation", evtWeight);

        if (!PassProbeIPCuts(probe)) continue;
        FillCutFlows("ProbeIP", evtWeight);

		// add energy deopsition in calo layers as decoration
		if ((m_addCaloDeposits) && (m_trk_calodep_tool->decorate(probe).isFailure())) {
		    ATH_MSG_WARNING("Unable to decorare probe with calo energy deposits!");
		}
		
        // for each selected probe build a Probe object
        probeCont->push_back(new Probe(*probe, *probe));
        have_probe = true;
    }


    ATH_MSG_DEBUG("Number of selected probes   : " << probeCont->size() );

    if (have_probe) FillCutFlows("HaveProbe",evtWeight);
    return probeCont;
}
//**********************************************************************


bool HighPtSingleMuonSelectionTool::PassProbeIsoCuts (const xAOD::IParticle* probe) const{
    const xAOD::Muon* probemu = dynamic_cast<const xAOD::Muon*>(probe);
    // ID probes: check ptcone 
    if (!probemu){
        ATH_CHECK(m_trk_iso_tool->decorate(probe));
    }
    if (m_probe_iso){
        if (!probemu){
            return  (m_probe_isotool->accept(*probe));
//            return (ptcone_rel < 0.06 && etcone_rel < 0.2);
        }
        if (probemu->author() == xAOD::Muon::MuidSA){
            ATH_MSG_DEBUG("Standalone muon. Outcome is "<<m_probe_isotool->accept(*probemu));
        }
        return  (m_probe_isotool->accept(*probemu));
    }
    else {
        return true;
    }
}

bool HighPtSingleMuonSelectionTool::PassProbeIPCuts(const xAOD::IParticle* probe) const{

    const xAOD::TrackParticle* tp = 0;
    const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
    if (muprobe) {
        tp = muprobe->primaryTrackParticle();
    }
    else {
        tp = dynamic_cast<const xAOD::TrackParticle*>(probe);
    }
    if (!tp){
        ATH_MSG_WARNING("Unable to find a track particle to cut on!");
    }
    return PassIPCuts(tp,m_probe_d0, m_probe_d0Sign, m_probe_z0);
}

bool HighPtSingleMuonSelectionTool::PassProbeKinematics (const xAOD::IParticle* probe) const{

    if(probe->pt() < m_probePtCut) return false;
    if(fabs(probe->eta()) > m_probeEtaCut) return false;
    return true;
}
