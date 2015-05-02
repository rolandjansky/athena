/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPTrees.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/ZmumuMuonTPIsolationTreeTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

ZmumuMuonTPIsolationTreeTool::ZmumuMuonTPIsolationTreeTool(std::string name)
      :  AsgTool(name),MuonTPTreeTool(name){
}

void ZmumuMuonTPIsolationTreeTool::FillCustomStuff(Probe& probe){

    const xAOD::VertexContainer* primVertices = 0 ;
    const xAOD::Vertex* pv = 0;
    if(evtStore()->retrieve(primVertices,"PrimaryVertices").isFailure()){
        ATH_MSG_ERROR("Found no PV candidate for IP computation!");
    }
    else {
        pv = primVertices->at(0);
    }

    m_Npvtx = primVertices->size();

    m_dPhi = fabs(probe.tagTrack().p4().DeltaPhi(probe.probeTrack().p4()));

	int charge = 0;
	// case of a muon probe
    charge = -2;
//     const xAOD::VertexContainer* vxc ("
	const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());
	if (muprobe) {
		charge = muprobe->trackParticle(xAOD::Muon::Primary)->charge() ;
        bool ok = muprobe->isolation(m_probeiso_etcone40,xAOD::Iso::topoetcone40);
        if (!ok) m_probeiso_etcone40 = -1;
        ok = muprobe->isolation(m_probeiso_ptcone40,xAOD::Iso::ptcone40);
        if (!ok) m_probeiso_ptcone40 = -1;
        ok = muprobe->isolation(m_probeiso_topoetcone40,xAOD::Iso::topoetcone40);
        if (!ok) m_probeiso_topoetcone40 = -1;
        ok = muprobe->isolation(m_probeiso_ptvarcone40,xAOD::Iso::ptvarcone40);
        if (!ok) m_probeiso_ptvarcone40 = -1;
        ok = muprobe->isolation(m_probeiso_neflowisol40,xAOD::Iso::neflowisol40);
        if (!ok) m_probeiso_neflowisol40 = -1;

        ok = muprobe->isolation(m_probeiso_etcone30,xAOD::Iso::topoetcone30);
        if (!ok) m_probeiso_etcone30 = -1;
        ok = muprobe->isolation(m_probeiso_ptcone30,xAOD::Iso::ptcone30);
        if (!ok) m_probeiso_ptcone30 = -1;
        ok = muprobe->isolation(m_probeiso_topoetcone30,xAOD::Iso::topoetcone30);
        if (!ok) m_probeiso_topoetcone30 = -1;
        ok = muprobe->isolation(m_probeiso_ptvarcone30,xAOD::Iso::ptvarcone30);
        if (!ok) m_probeiso_ptvarcone30 = -1;
        ok = muprobe->isolation(m_probeiso_neflowisol30,xAOD::Iso::neflowisol30);
        if (!ok) m_probeiso_neflowisol30 = -1;

        ok = muprobe->isolation(m_probeiso_etcone20,xAOD::Iso::topoetcone20);
        if (!ok) m_probeiso_etcone20 = -1;
        ok = muprobe->isolation(m_probeiso_ptcone20,xAOD::Iso::ptcone20);
        if (!ok) m_probeiso_ptcone20 = -1;
        ok = muprobe->isolation(m_probeiso_topoetcone20,xAOD::Iso::topoetcone20);
        if (!ok) m_probeiso_topoetcone20 = -1;
        ok = muprobe->isolation(m_probeiso_ptvarcone20,xAOD::Iso::ptvarcone20);
        if (!ok) m_probeiso_ptvarcone40 = -1;
        ok = muprobe->isolation(m_probeiso_neflowisol20,xAOD::Iso::neflowisol20);
        if (!ok) m_probeiso_neflowisol20 = -1;

        if (muprobe->primaryTrackParticle()){
            m_d0 = muprobe->primaryTrackParticle()->d0();
            m_d0err = sqrt(muprobe->primaryTrackParticle()->definingParametersCovMatrix()(0,0));
            m_z0 = muprobe->primaryTrackParticle()->z0();
            if (pv){
                m_z0 = m_z0 - pv->z() + muprobe->primaryTrackParticle()->vz();
            }
        }
	}


        const xAOD::Muon* mutag = dynamic_cast<const xAOD::Muon*>(&probe.tagTrack());

	int charge2 = 0;
	charge2 = -2;

        if (mutag) {
                charge2 = mutag->trackParticle(xAOD::Muon::Primary)->charge() ;

        bool ok2 = mutag->isolation(m_tagiso_etcone40,xAOD::Iso::topoetcone40);
        if (!ok2) m_tagiso_etcone40 = -1;
        ok2 = mutag->isolation(m_tagiso_ptcone40,xAOD::Iso::ptcone40);
        if (!ok2) m_tagiso_ptcone40 = -1;
        ok2 = mutag->isolation(m_tagiso_topoetcone40,xAOD::Iso::topoetcone40);
        if (!ok2) m_tagiso_topoetcone40 = -1;
        ok2 = mutag->isolation(m_tagiso_ptvarcone40,xAOD::Iso::ptvarcone40);
        if (!ok2) m_tagiso_ptvarcone40 = -1;
        ok2 = mutag->isolation(m_tagiso_neflowisol40,xAOD::Iso::neflowisol40);
        if (!ok2) m_tagiso_neflowisol40 = -1;

        ok2 = mutag->isolation(m_tagiso_etcone30,xAOD::Iso::topoetcone30);
        if (!ok2) m_tagiso_etcone30 = -1;
        ok2 = mutag->isolation(m_tagiso_ptcone30,xAOD::Iso::ptcone30);
        if (!ok2) m_tagiso_ptcone30 = -1;
        ok2 = mutag->isolation(m_tagiso_topoetcone30,xAOD::Iso::topoetcone30);
        if (!ok2) m_tagiso_topoetcone30 = -1;
        ok2 = mutag->isolation(m_tagiso_ptvarcone30,xAOD::Iso::ptvarcone30);
        if (!ok2) m_tagiso_ptvarcone30 = -1;
        ok2 = mutag->isolation(m_tagiso_neflowisol30,xAOD::Iso::neflowisol30);
        if (!ok2) m_tagiso_neflowisol30 = -1;

        ok2 = mutag->isolation(m_tagiso_etcone20,xAOD::Iso::topoetcone20);
        if (!ok2) m_tagiso_etcone20 = -1;
        ok2 = mutag->isolation(m_tagiso_ptcone20,xAOD::Iso::ptcone20);
        if (!ok2) m_tagiso_ptcone20 = -1;
        ok2 = mutag->isolation(m_tagiso_topoetcone20,xAOD::Iso::topoetcone20);
        if (!ok2) m_tagiso_topoetcone20 = -1;
        ok2 = mutag->isolation(m_tagiso_ptvarcone20,xAOD::Iso::ptvarcone20);
        if (!ok2) m_tagiso_ptvarcone20 = -1;
        ok2 = mutag->isolation(m_tagiso_neflowisol20,xAOD::Iso::neflowisol20);
        if (!ok2) m_tagiso_neflowisol20 = -1;
	}

	// case of an ID probe
	const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(&probe.probeTrack());
	if (!muprobe && trkprobe){
	    charge = trkprobe->charge();
        if (trkprobe->isAvailable<float>("TopoEtCone40")){
            m_probeiso_etcone40 =  trkprobe->auxdataConst<float>("TopoEtCone40");
        }
        else {
            m_probeiso_etcone40 = -1.;
        }
        if (trkprobe->isAvailable<float>("PtCone40")){
            m_probeiso_ptcone40 =  trkprobe->auxdataConst<float>("PtCone40");
        }
        else {
            m_probeiso_ptcone40 = -1.;
        }
        m_d0 = trkprobe->d0();
        m_d0err = sqrt(trkprobe->definingParametersCovMatrix()(0,0));
        m_z0 = trkprobe->z0();
//         const xAOD::Vertex* vx = trkprobe->vertex();
        if (pv){
            m_z0 = m_z0 - pv->z() + trkprobe->vz();
        }
	}

    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info).isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }
    static bool isMC = info->eventType(xAOD::EventInfo::IS_SIMULATION);

	m_runNumber = info->runNumber();
	m_eventNumber = info->eventNumber();
	m_mu = info->averageInteractionsPerCrossing();
	m_mcEventWeight = (isMC ? info->mcEventWeight() : 1.00);
    m_probePt = probe.pt() / 1000.;
    m_probeEta = probe.eta();
    m_probePhi = probe.phi();

    m_fineEtaPhi = m_fepb.bin(probe.probeTrack().p4());
    m_probe_q = charge;
    m_tag_q = charge2;
    m_tagPt = probe.tagTrack().pt() / 1000.;
    m_tagEta = probe.tagTrack().eta();
    m_tagPhi = probe.tagTrack().phi();
    m_mll = (probe.tagTrack().p4()+probe.probeTrack().p4()).M();
    
    
}
void ZmumuMuonTPIsolationTreeTool::AddCustomBranches(TTree* tree){

	tree->Branch("runNumber",&m_runNumber);
	tree->Branch("eventNumber",&m_eventNumber);
	tree->Branch("mu",&m_mu);
	tree->Branch("mcEventWeight",&m_mcEventWeight);
    tree->Branch("tag_pt",&m_tagPt);
    tree->Branch("tag_eta",&m_tagEta);
    tree->Branch("tag_phi",&m_tagPhi);
    tree->Branch("tagiso_neflowisol20",&m_tagiso_neflowisol20);
    tree->Branch("tagiso_topoetcone20",&m_tagiso_topoetcone20);
    tree->Branch("tagiso_etcone20",&m_tagiso_etcone20);
    tree->Branch("tagiso_ptcone20",&m_tagiso_ptcone20);
    tree->Branch("tagiso_ptvarcone20",&m_tagiso_ptvarcone20);
    tree->Branch("tagiso_neflowisol30",&m_tagiso_neflowisol30);
    tree->Branch("tagiso_topoetcone30",&m_tagiso_topoetcone30);
    tree->Branch("tagiso_etcone30",&m_tagiso_etcone30);
    tree->Branch("tagiso_ptcone30",&m_tagiso_ptcone30);
    tree->Branch("tagiso_ptvarcone30",&m_tagiso_ptvarcone30);
    tree->Branch("tagiso_neflowisol40",&m_tagiso_neflowisol40);
    tree->Branch("tagiso_topoetcone40",&m_tagiso_topoetcone40);
    tree->Branch("tagiso_etcone40",&m_tagiso_etcone40);
    tree->Branch("tagiso_ptcone40",&m_tagiso_ptcone40);
    tree->Branch("tagiso_ptvarcone40",&m_tagiso_ptvarcone40);
    tree->Branch("probe_pt",&m_probePt);
    tree->Branch("probe_eta",&m_probeEta);
    tree->Branch("probe_phi",&m_probePhi);
    tree->Branch("probeiso_neflowisol20",&m_probeiso_neflowisol20);
    tree->Branch("probeiso_topoetcone20",&m_probeiso_topoetcone20);
    tree->Branch("probeiso_etcone20",&m_probeiso_etcone20);
    tree->Branch("probeiso_ptcone20",&m_probeiso_ptcone20);
    tree->Branch("probeiso_ptvarcone20",&m_probeiso_ptvarcone20);
    tree->Branch("probeiso_neflowisol30",&m_probeiso_neflowisol30);
    tree->Branch("probeiso_topoetcone30",&m_probeiso_topoetcone30);
    tree->Branch("probeiso_etcone30",&m_probeiso_etcone30);
    tree->Branch("probeiso_ptcone30",&m_probeiso_ptcone30);
    tree->Branch("probeiso_ptvarcone30",&m_probeiso_ptvarcone30);
    tree->Branch("probeiso_neflowisol40",&m_probeiso_neflowisol40);
    tree->Branch("probeiso_topoetcone40",&m_probeiso_topoetcone40);
    tree->Branch("probeiso_etcone40",&m_probeiso_etcone40);
    tree->Branch("probeiso_ptcone40",&m_probeiso_ptcone40);
    tree->Branch("probeiso_ptvarcone40",&m_probeiso_ptvarcone40);
    tree->Branch("d0",&m_d0);
    tree->Branch("d0err",&m_d0err);
    tree->Branch("z0",&m_z0);
    tree->Branch("fineEtaPhi",&m_fineEtaPhi);
    tree->Branch("tag_q",&m_tag_q);
    tree->Branch("probe_q",&m_probe_q);
    tree->Branch("mll",&m_mll);
}
