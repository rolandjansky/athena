/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPTrees.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/DiMuonTPTreeTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventShape/EventShape.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODJet/JetContainer.h"

DiMuonTPTreeTool::DiMuonTPTreeTool(std::string name)
  :  MuonTPTreeTool(name),
     m_runNumber(0),
     m_eventNumber(0),
     m_lumiblock(0),
     m_average_mu(-1.),
     m_actual_mu(-1),
     m_PV_n(-1),
     m_mcEventWeight(-1),
     m_pt(-1),
     m_eta(-1),
     m_phi(-1),
     m_type(-1),
     m_author(-1),
     m_quality(-1),
     m_fineEtaPhi(-1),
     m_detregion(-1),
     m_q(-1),
     m_d0(-1),
     m_d0err(-1),
     m_z0(-1),
     m_tagPt(-1),
     m_tagEta(-1),
     m_tagPhi(-1),
     m_tag_q(-1),
     m_tag_d0(-1),
     m_tag_d0err(-1),
     m_tag_z0(-1),
     m_mll(-1),
     m_dPhi(-1),
     m_dilep_pt(-1),
     m_dilep_tof(-1),
     m_tag_deltaR(-1),
     m_deltaR(-1),
     m_tagiso_neflowisol20(-1),
     m_tagiso_topoetcone20(-1),
     m_tagiso_ptcone20(-1),
     m_tagiso_etcone20(-1),
     m_tagiso_ptvarcone20(-1),

     m_tagiso_neflowisol30(-1),
     m_tagiso_topoetcone30(-1),
     m_tagiso_ptcone30(-1),
     m_tagiso_etcone30(-1),
     m_tagiso_ptvarcone30(-1),

     m_tagiso_neflowisol40(-1),
     m_tagiso_topoetcone40(-1),
     m_tagiso_ptcone40(-1),
     m_tagiso_etcone40(-1),
     m_tagiso_ptvarcone40(-1),

     m_tag_topocore(-1),

     m_probeiso_neflowisol20(-1),
     m_probeiso_topoetcone20(-1),
     m_probeiso_ptcone20(-1),
     m_probeiso_etcone20(-1),
     m_probeiso_ptvarcone20(-1),

     m_probeiso_neflowisol30(-1),
     m_probeiso_topoetcone30(-1),
     m_probeiso_ptcone30(-1),
     m_probeiso_etcone30(-1),
     m_probeiso_ptvarcone30(-1),

     m_probeiso_neflowisol40(-1),
     m_probeiso_topoetcone40(-1),
     m_probeiso_ptcone40(-1),
     m_probeiso_etcone40(-1),
     m_probeiso_ptvarcone40(-1),

     m_probe_topocore(-1),

     m_energyDensity_central(-1),
     m_energyDensity_forward(-1)
{
  declareProperty("MuonSelectionTool",m_muon_selection_tool);  
  declareProperty("AddExtendedIsolation",m_extended_Iso_Info = false);
}

StatusCode DiMuonTPTreeTool::initialize()
{
  ATH_CHECK(m_muon_selection_tool.retrieve());
  return StatusCode::SUCCESS;

}
void DiMuonTPTreeTool::FillCustomStuff(Probe& probe){

  m_energyDensity_central = 0.;
  m_energyDensity_forward = 0.;
  m_tag_topocore = 0.;
  m_probe_topocore = 0.;

  // Primary vertex variables
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

  // distance to clostes jet
  m_deltaR = 99.;
  m_tag_deltaR = 99.;
  const xAOD::JetContainer* jets = 0;
  if (evtStore()->retrieve(jets,"AntiKt4LCTopoJets").isFailure()){
    ATH_MSG_WARNING( "Unable to retrieve Jet Container" );
  }else{
    // calculate delta R to the closest jet        
    for(auto jet : *jets) {
      float probe_deltaR = probe.probeTrack().p4().DeltaR(jet->p4());
      if (probe_deltaR < m_deltaR) m_deltaR = probe_deltaR;
      float tag_deltaR = probe.tagTrack().p4().DeltaR(jet->p4());
      if (tag_deltaR < m_tag_deltaR) m_tag_deltaR = tag_deltaR;      
    }
    if (jets->size()==0) {
      m_deltaR=-1;
      m_tag_deltaR=-1;
    }
  }

  // Event shape variables for isolation
  const xAOD::EventShape* edShape = 0;
  double rho = 0.;
  if (evtStore()->retrieve(edShape,"TopoClusterIsoCentralEventShape").isFailure()){  // Returns a boolean, needs to be surrounded with error checks.
    ATH_MSG_FATAL( "Unable to retrieve TopoClusterIsoCentralEventShape" );
  } else {
    edShape->getDensity(xAOD::EventShape::Density,rho); // Returns a boolean, needs to be surrounded with error checks.
    m_energyDensity_central = rho;
  }
  if (evtStore()->retrieve(edShape,"TopoClusterIsoForwardEventShape").isFailure()){  // Returns a boolean, needs to be surrounded with error checks.
    ATH_MSG_FATAL( "Unable to retrieve TopoClusterIsoForwardEventShape" );
  } else {
    edShape->getDensity(xAOD::EventShape::Density,rho); // Returns a boolean, needs to be surrounded with error checks.
    m_energyDensity_forward = rho;
  }
    
    
  int charge = -2;
  m_type = 0;
  m_author = -1;
  m_quality = -1;
  const xAOD::Vertex* vx_tag = 0;
  const xAOD::Vertex* vx_probe = 0;
  // tag muon properties 
  const xAOD::Muon* mutag = dynamic_cast<const xAOD::Muon*>(&probe.tagTrack());
  int charge2 = -2;
  if (mutag) {
    charge2 = mutag->trackParticle(xAOD::Muon::Primary)->charge();
    vx_tag = mutag->trackParticle(xAOD::Muon::Primary)->vertex();
        
    if (mutag->primaryTrackParticle()){
      m_tag_d0 = mutag->primaryTrackParticle()->d0();
      m_tag_d0err = sqrt(mutag->primaryTrackParticle()->definingParametersCovMatrix()(0,0));
      m_tag_z0 = mutag->primaryTrackParticle()->z0();
      if (pv){
	m_tag_z0 = m_tag_z0 - pv->z() + mutag->primaryTrackParticle()->vz();
      }
    }
    if (m_extended_Iso_Info){
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
  }
    
    
  // case of a muon probe
  const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());
  if (muprobe) {
    charge = muprobe->trackParticle(xAOD::Muon::Primary)->charge() ;
    if (muprobe->primaryTrackParticle()){
      m_d0 = muprobe->primaryTrackParticle()->d0();
      m_d0err = sqrt(muprobe->primaryTrackParticle()->definingParametersCovMatrix()(0,0));
      m_z0 = muprobe->primaryTrackParticle()->z0();
      if (pv){
	m_z0 = m_z0 - pv->z() + muprobe->primaryTrackParticle()->vz();
      }
    }
    vx_probe = muprobe->trackParticle(xAOD::Muon::Primary)->vertex();
    muprobe->isolationCaloCorrection(m_probe_topocore, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
    xAOD::Muon::Quality probe_qual = m_muon_selection_tool->getQuality(*muprobe);
    if ( probe_qual == xAOD::Muon::Tight ) m_quality = 0; 
    else if ( probe_qual == xAOD::Muon::Medium ) m_quality = 1; 
    else if ( probe_qual == xAOD::Muon::Loose ) m_quality = 2; 
    else if ( probe_qual == xAOD::Muon::VeryLoose ) m_quality = 3;
        
    if ( muprobe->author() == xAOD::Muon::unknown ) m_author = 0; 
    else if ( muprobe->author() == xAOD::Muon::MuidCo ) m_author = 1; 
    else if ( muprobe->author() == xAOD::Muon::STACO ) m_author = 2; 
    else if ( muprobe->author() == xAOD::Muon::MuTag ) m_author = 3; 
    else if ( muprobe->author() == xAOD::Muon::MuTagIMO ) m_author = 4; 
    else if ( muprobe->author() == xAOD::Muon::MuidSA ) m_author = 5; 
    else if ( muprobe->author() == xAOD::Muon::MuGirl ) m_author = 6; 
    else if ( muprobe->author() == xAOD::Muon::MuGirlLowBeta ) m_author = 7; 
    else if ( muprobe->author() == xAOD::Muon::CaloTag ) m_author = 8; 
    else if ( muprobe->author() == xAOD::Muon::CaloLikelihood ) m_author = 9; 
    else if ( muprobe->author() == xAOD::Muon::ExtrapolateMuonToIP ) m_author = 10; 
        
    if ( muprobe->muonType() == xAOD::Muon::Combined ) m_type = 1; // "CB"
    else if ( muprobe->muonType() == xAOD::Muon::MuonStandAlone ) m_type = 2; // "SA"
    else if ( muprobe->muonType() == xAOD::Muon::SegmentTagged ) m_type = 3; // "ST"
    else if ( muprobe->muonType() == xAOD::Muon::CaloTagged ) m_type = 4; // "CT"
        
    if (m_extended_Iso_Info){
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
    }
  }

  // case of an ID probe
  const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(&probe.probeTrack());
  if (!muprobe && trkprobe){
    charge = trkprobe->charge();
    vx_probe = trkprobe->vertex();
    m_d0 = trkprobe->d0();
    m_d0err = sqrt(trkprobe->definingParametersCovMatrix()(0,0));
    m_z0 = trkprobe->z0();
    //         const xAOD::Vertex* vx = trkprobe->vertex();
    if (pv){
      m_z0 = m_z0 - pv->z() + trkprobe->vz();
    }
  }
  // case of an truth probe: rely on the sign of the pdg ID
  const xAOD::TruthParticle* truthtrk = dynamic_cast<const xAOD::TruthParticle*>(&probe.probeTrack());
  if (truthtrk) {
    if (truthtrk->charge() > 0) charge = 1.0;
    if (truthtrk->charge() < 0) charge = -1.0;
        
  }

  const xAOD::EventInfo* info = 0;
  ATH_MSG_DEBUG(""<<evtStore());
  if (evtStore()->retrieve(info, "EventInfo").isFailure()){
    ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  }
  static bool isMC = info->eventType(xAOD::EventInfo::IS_SIMULATION);

  m_runNumber = info->runNumber();
  m_eventNumber = info->eventNumber();
  m_lumiblock = info->lumiBlock();
  m_average_mu = info->averageInteractionsPerCrossing();
  m_actual_mu = info->actualInteractionsPerCrossing();
  m_mcEventWeight = (isMC ? info->mcEventWeight() : 1.00);
  m_pt = probe.pt() / 1000.;
  m_eta = probe.eta();
  m_phi = probe.phi();
  m_fineEtaPhi = m_fepb.bin(probe.probeTrack().p4());
  m_detregion = m_epb.symmetricBin(probe.probeTrack().p4());
  m_q = charge;
  m_tag_q = charge2;
  m_tagPt = probe.tagTrack().pt() / 1000.;
  m_tagEta = probe.tagTrack().eta();
  m_tagPhi = probe.tagTrack().phi();
  m_mll = (probe.tagTrack().p4()+probe.probeTrack().p4()).M();
  m_dilep_pt = (probe.tagTrack().p4()+probe.probeTrack().p4()).Pt();
    
  // if we have a valid decay vertex and primary vertex, compute the time of flight
  if (vx_tag && vx_tag == vx_probe && pv){
    TVector3 vvx_tag (vx_tag->x(),vx_tag->y(),vx_tag->z());
    TVector3 vpv (pv->x(),pv->y(),pv->z());
    m_dilep_tof = (vvx_tag - vpv).Perp() * 3096. / m_dilep_pt;
  }
  else{
    m_dilep_tof = -1;
  }
  m_dPhi = probe.tagTrack().p4().DeltaPhi(probe.probeTrack().p4());
    
    
}
void DiMuonTPTreeTool::AddCustomBranches(TTree* tree){

  tree->Branch("runNumber",&m_runNumber);
  tree->Branch("eventNumber",&m_eventNumber);
  tree->Branch("lumiblock",&m_lumiblock);
  tree->Branch("average_mu",&m_average_mu);
  tree->Branch("actual_mu",&m_actual_mu);
  tree->Branch("PV_n",&m_PV_n);
  tree->Branch("mcEventWeight",&m_mcEventWeight);
  tree->Branch("probe_pt",&m_pt);
  tree->Branch("probe_eta",&m_eta);
  tree->Branch("probe_phi",&m_phi);
  tree->Branch("probe_type",&m_type);
  tree->Branch("probe_author",&m_author);
  tree->Branch("probe_quality",&m_quality);
  tree->Branch("probe_fineEtaPhi",&m_fineEtaPhi);
  tree->Branch("probe_detRegion",&m_detregion);
  tree->Branch("probe_q",&m_q);
  tree->Branch("probe_d0",&m_d0);
  tree->Branch("probe_d0err",&m_d0err);
  tree->Branch("probe_z0",&m_z0);
  tree->Branch("tag_pt",&m_tagPt);
  tree->Branch("tag_eta",&m_tagEta);
  tree->Branch("tag_phi",&m_tagPhi);
  tree->Branch("tag_q",&m_tag_q);
  tree->Branch("tag_d0",&m_tag_d0);
  tree->Branch("tag_d0err",&m_tag_d0err);
  tree->Branch("tag_z0",&m_tag_z0);
  tree->Branch("dilep_mll",&m_mll);
  tree->Branch("dilep_dphi",&m_dPhi);
  tree->Branch("dilep_pt",&m_dilep_pt);
  tree->Branch("dilep_tof",&m_dilep_tof);

  tree->Branch("tag_dRJet",&m_tag_deltaR);
  tree->Branch("probe_dRJet",&m_deltaR);
  
  if (m_extended_Iso_Info){
    tree->Branch("tag_neflowisol20",&m_tagiso_neflowisol20);
    tree->Branch("tag_topoetcone20",&m_tagiso_topoetcone20);
    tree->Branch("tag_ptcone20",&m_tagiso_ptcone20);
    tree->Branch("tag_etcone20",&m_tagiso_etcone20);
    tree->Branch("tag_ptvarcone20",&m_tagiso_ptvarcone20);

    tree->Branch("tag_neflowisol30",&m_tagiso_neflowisol30);
    tree->Branch("tag_topoetcone30",&m_tagiso_topoetcone30);
    tree->Branch("tag_ptcone30",&m_tagiso_ptcone30);
    tree->Branch("tag_etcone30",&m_tagiso_etcone30);
    tree->Branch("tag_ptvarcone30",&m_tagiso_ptvarcone30);

    tree->Branch("tag_neflowisol40",&m_tagiso_neflowisol40);
    tree->Branch("tag_topoetcone40",&m_tagiso_topoetcone40);
    tree->Branch("tag_ptcone40",&m_tagiso_ptcone40);
    tree->Branch("tag_etcone40",&m_tagiso_etcone40);
    tree->Branch("tag_ptvarcone40",&m_tagiso_ptvarcone40);

    tree->Branch("tag_topocore",&m_tag_topocore);

    tree->Branch("probe_neflowisol20",&m_probeiso_neflowisol20);
    tree->Branch("probe_topoetcone20",&m_probeiso_topoetcone20);
    tree->Branch("probe_ptcone20",&m_probeiso_ptcone20);
    tree->Branch("probe_etcone20",&m_probeiso_etcone20);
    tree->Branch("probe_ptvarcone20",&m_probeiso_ptvarcone20);

    tree->Branch("probe_neflowisol30",&m_probeiso_neflowisol30);
    tree->Branch("probe_topoetcone30",&m_probeiso_topoetcone30);
    tree->Branch("probe_ptcone30",&m_probeiso_ptcone30);
    tree->Branch("probe_etcone30",&m_probeiso_etcone30);
    tree->Branch("probe_ptvarcone30",&m_probeiso_ptvarcone30);

    tree->Branch("probe_neflowisol40",&m_probeiso_neflowisol40);
    tree->Branch("probe_topoetcone40",&m_probeiso_topoetcone40);
    tree->Branch("probe_ptcone40",&m_probeiso_ptcone40);
    tree->Branch("probe_etcone40",&m_probeiso_etcone40);
    tree->Branch("probe_ptvarcone40",&m_probeiso_ptvarcone40);

    tree->Branch("probe_topocore",&m_probe_topocore);

    tree->Branch("energyDensity_central",&m_energyDensity_central);
    tree->Branch("energyDensity_forward",&m_energyDensity_forward);
  }

}
