/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPTrees.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/ZmumuMuonTPTreeTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTruth/TruthParticle.h"

ZmumuMuonTPTreeTool::ZmumuMuonTPTreeTool(std::string name)
      :  AsgTool(name),MuonTPTreeTool(name){
}

void ZmumuMuonTPTreeTool::FillCustomStuff(Probe& probe){

    const xAOD::VertexContainer* primVertices = 0 ;
    const xAOD::Vertex* pv = 0;
    if(evtStore()->retrieve(primVertices,"PrimaryVertices").isFailure()){
        ATH_MSG_ERROR("Found no PV candidate for IP computation!");
    }
    else {
        pv = primVertices->at(0);
    }
    m_PV_n=0;
    for (auto vx: *primVertices){
        if (vx->vertexType() == xAOD::VxType::PriVtx || vx->vertexType() == xAOD::VxType::PileUp) ++m_PV_n;
    }
	int charge = 0;
	// case of a muon probe
    charge = -2;
    m_ptcone40 = -1;
    m_etcone40 = -1;
//     const xAOD::VertexContainer* vxc ("
	const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());
	if (muprobe) {
		charge = muprobe->trackParticle(xAOD::Muon::Primary)->charge() ;
        bool ok = muprobe->isolation(m_etcone40,xAOD::Iso::topoetcone40);
        if (!ok) m_etcone40 = -1;
        ok = muprobe->isolation(m_ptcone40,xAOD::Iso::ptcone40);
        if (!ok) m_ptcone40 = -1;
        if (muprobe->primaryTrackParticle()){
            m_d0 = muprobe->primaryTrackParticle()->d0();
            m_d0err = sqrt(muprobe->primaryTrackParticle()->definingParametersCovMatrix()(0,0));
            m_z0 = muprobe->primaryTrackParticle()->z0();
            if (pv){
                m_z0 = m_z0 - pv->z() + muprobe->primaryTrackParticle()->vz();
            }
            muprobe->primaryTrackParticle()->summaryValue(m_nBL, xAOD::numberOfBLayerHits);
            muprobe->primaryTrackParticle()->summaryValue(m_nPIX, xAOD::numberOfPixelHits);
            muprobe->primaryTrackParticle()->summaryValue(m_nPIXholes, xAOD::numberOfPixelHoles);
            muprobe->primaryTrackParticle()->summaryValue(m_nPIXdead, xAOD::numberOfPixelDeadSensors);
            muprobe->primaryTrackParticle()->summaryValue(m_nSCT, xAOD::numberOfSCTHits );
            muprobe->primaryTrackParticle()->summaryValue(m_nSCTholes, xAOD::numberOfSCTHoles  );
            muprobe->primaryTrackParticle()->summaryValue(m_nSCTdead, xAOD::numberOfSCTDeadSensors  );
            muprobe->primaryTrackParticle()->summaryValue(m_nTRT, xAOD::numberOfTRTHits   );
            muprobe->primaryTrackParticle()->summaryValue(m_nTRTout, xAOD::numberOfTRTOutliers    );
        }
	}
	// case of an ID probe
	const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(&probe.probeTrack());
	if (!muprobe && trkprobe){
	    charge = trkprobe->charge();
        if (trkprobe->isAvailable<float>("MUON1_topoetcone40")){
            m_etcone40 =  trkprobe->auxdataConst<float>("MUON1_topoetcone40");
        }
        else {
            m_etcone40 = -1.;
        }
        if (trkprobe->isAvailable<float>("MUON1_ptcone40")){
            m_ptcone40 =  trkprobe->auxdataConst<float>("MUON1_ptcone40");
        }
        else {
            m_ptcone40 = -1.;
        }
        m_d0 = trkprobe->d0();
        m_d0err = sqrt(trkprobe->definingParametersCovMatrix()(0,0));
        m_z0 = trkprobe->z0();
//         const xAOD::Vertex* vx = trkprobe->vertex();
        if (pv){
            m_z0 = m_z0 - pv->z() + trkprobe->vz();
        }
        trkprobe->summaryValue(m_nBL, xAOD::numberOfBLayerHits);
        trkprobe->summaryValue(m_nPIX, xAOD::numberOfPixelHits);
        trkprobe->summaryValue(m_nPIXholes, xAOD::numberOfPixelHoles);
        trkprobe->summaryValue(m_nPIXdead, xAOD::numberOfPixelDeadSensors);
        trkprobe->summaryValue(m_nSCT, xAOD::numberOfSCTHits );
        trkprobe->summaryValue(m_nSCTholes, xAOD::numberOfSCTHoles  );
        trkprobe->summaryValue(m_nSCTdead, xAOD::numberOfSCTDeadSensors  );
        trkprobe->summaryValue(m_nTRT, xAOD::numberOfTRTHits   );
        trkprobe->summaryValue(m_nTRTout, xAOD::numberOfTRTOutliers    );
	}
    // case of an truth probe: rely on the sign of the pdg ID
    const xAOD::TruthParticle* truthtrk = dynamic_cast<const xAOD::TruthParticle*>(&probe.probeTrack());
    if (truthtrk) {
        if (truthtrk->charge() > 0) charge = 1.0;
        if (truthtrk->charge() < 0) charge = -1.0;
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
    m_pt = probe.pt() / 1000.;
    m_eta = probe.eta();
    m_phi = probe.phi();
    m_fineEtaPhi = m_fepb.bin(probe.probeTrack().p4());
    m_detregion = m_epb.symmetricBin(probe.probeTrack().p4());
    m_q = charge;
    m_tagPt = probe.tagTrack().pt() / 1000.;
    m_tagEta = probe.tagTrack().eta();
    m_tagPhi = probe.tagTrack().phi();
    m_mll = (probe.tagTrack().p4()+probe.probeTrack().p4()).M();
    
    
}
void ZmumuMuonTPTreeTool::AddCustomBranches(TTree* tree){

	tree->Branch("runNumber",&m_runNumber);
	tree->Branch("eventNumber",&m_eventNumber);
	tree->Branch("mu",&m_mu);
	tree->Branch("mcEventWeight",&m_mcEventWeight);
    tree->Branch("pt",&m_pt);
    tree->Branch("eta",&m_eta);
    tree->Branch("phi",&m_phi);
    tree->Branch("d0",&m_d0);
    tree->Branch("d0err",&m_d0err);
    tree->Branch("z0",&m_z0);
    tree->Branch("ptcone40",&m_ptcone40);
    tree->Branch("etcone40",&m_etcone40);
    tree->Branch("fineEtaPhi",&m_fineEtaPhi);
    tree->Branch("detRegion",&m_detregion);
    tree->Branch("q",&m_q);
    tree->Branch("tagPt",&m_tagPt);
    tree->Branch("tagEta",&m_tagEta);
    tree->Branch("tagPhi",&m_tagPhi);
    tree->Branch("mll",&m_mll);
    tree->Branch("PV_n",&m_PV_n);

    tree->Branch("nBL",&m_nBL);
    tree->Branch("nPIX",&m_nPIX);
    tree->Branch("nSCT",&m_nSCT);
    tree->Branch("nPIXdead",&m_nPIXdead);
    tree->Branch("nSCTdead",&m_nSCTdead);
    tree->Branch("nSCTholes",&m_nSCTholes);
    tree->Branch("nPIXholes",&m_nPIXholes);
    tree->Branch("nTRT",&m_nTRT);
    tree->Branch("nTRTout",&m_nTRTout);

}
