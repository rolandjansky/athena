/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXNtupleWriter.cxx  -  
//                              -------------------
//     begin                : 28 02 2020
//     email                : tong.qiu@cern.ch
//  **************************************************************************

#include <L1CaloFEXSim/eFEXNtupleWriter.h>
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/eFEXegTOB.h"
#include "L1CaloFEXSim/eFEXtauAlgo.h"
#include "L1CaloFEXSim/eFEXtauTOB.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"
#include <vector>
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <memory>

#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODJet/JetContainer.h"


LVL1::eFEXNtupleWriter::eFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator) { }

LVL1::eFEXNtupleWriter::~eFEXNtupleWriter() {

}

StatusCode LVL1::eFEXNtupleWriter::initialize () {
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
  CHECK( histSvc.retrieve() );
  m_myTree = new TTree("data","data");
  CHECK( histSvc->regTree("/ANALYSIS/data",m_myTree) );
  
  m_load_truth_jet = false;

  m_myTree->Branch ("truth_tauvisible_eta",  &m_truth_tauvisible_eta);
  m_myTree->Branch ("truth_tauvisible_phi",  &m_truth_tauvisible_phi);
  m_myTree->Branch ("truth_tauvisible_ET",  &m_truth_tauvisible_ET);

  m_myTree->Branch ("truth_e_eta",  &m_truth_e_eta);
  m_myTree->Branch ("truth_e_phi",  &m_truth_e_phi);
  m_myTree->Branch ("truth_e_ET",  &m_truth_e_ET);
  if (m_load_truth_jet){
    m_myTree->Branch ("truth_jet_eta",  &m_truth_jet_eta);
    m_myTree->Branch ("truth_jet_phi",  &m_truth_jet_phi);
    m_myTree->Branch ("truth_jet_ET",  &m_truth_jet_ET);
  }

  m_myTree->Branch ("em",  &m_em);
  m_myTree->Branch ("had",  &m_had);

  m_myTree->Branch ("eg_eta",  &m_eg_eta);
  m_myTree->Branch ("eg_phi",  &m_eg_phi);
  m_myTree->Branch ("eg_ET",  &m_eg_ET);
  m_myTree->Branch ("eg_wstotnum",  &m_eg_WstotNum);
  m_myTree->Branch ("eg_wstotden",  &m_eg_WstotDen);
  m_myTree->Branch ("eg_retanum",  &m_eg_RetaNum);
  m_myTree->Branch ("eg_retaden",  &m_eg_RetaDen);
  m_myTree->Branch ("eg_rhadnum",  &m_eg_RhadNum);
  m_myTree->Branch ("eg_rhadden",  &m_eg_RhadDen);
  m_myTree->Branch ("eg_nTOBs",  &m_eg_nTOBs, "nTOBs");
  m_myTree->Branch ("eg_haveseed",  &m_eg_haveseed);

  m_myTree->Branch ("tau_Iso",  &m_tau_Iso);
  m_myTree->Branch ("tau_Et",  &m_tau_Et);
  m_myTree->Branch ("tau_isCentralTowerSeed",  &m_tau_isCentralTowerSeed);

  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXNtupleWriter::execute () {
  //ATH_MSG_DEBUG("==== eFEXNtupleWriter ============ execute()");
  ServiceHandle<StoreGateSvc> evtStore("StoreGateSvc/StoreGateSvc",  "arbitrary");
  CHECK(evtStore.retrieve() );

  m_eFEXOutputCollection = new eFEXOutputCollection();
  //m_eFEXOutputCollection = std::make_shared<eFEXOutputCollection>();
  CHECK(evtStore->retrieve(m_eFEXOutputCollection, "eFEXOutputCollection"));

  CHECK(loadegAlgoVariables());
  CHECK(loadtauAlgoVariables());
  CHECK(loadTruthElectron());
  CHECK(loadTruthTau());
  if (m_load_truth_jet){
    CHECK(loadTruthJets());
  }

  m_myTree->Fill();
  m_eFEXOutputCollection->clear();
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXNtupleWriter::finalize () {
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXNtupleWriter::loadtauAlgoVariables() {
  m_tau_Iso.clear();
  m_tau_Et.clear();
  m_tau_isCentralTowerSeed.clear();
  for (int i = 0; i < m_eFEXOutputCollection->size(); i++)
  {
    m_tau_isCentralTowerSeed.push_back((*(m_eFEXOutputCollection->get_tau(i)))["isCentralTowerSeed"]);
    m_tau_Et.push_back((*(m_eFEXOutputCollection->get_tau(i)))["Et"]);
    m_tau_Iso.push_back((*(m_eFEXOutputCollection->get_tau(i)))["Iso"]);
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXNtupleWriter::loadegAlgoVariables() {
  m_eg_ET.clear();
  m_eg_WstotNum.clear();
  m_eg_WstotDen.clear();
  m_eg_eta.clear();
  m_eg_phi.clear();
  m_eg_haveseed.clear();
  m_eg_RetaNum.clear();
  m_eg_RetaDen.clear();
  m_eg_RhadNum.clear();
  m_eg_RhadDen.clear();

  m_em.clear();
  m_had.clear();
  m_eg_nTOBs = m_eFEXOutputCollection->size();
  for (int i = 0; i < m_eFEXOutputCollection->size(); i++)
  {
    std::map<std::string, float> eFEXegvalue_tem = (*(m_eFEXOutputCollection->get_eg(i)));
    m_eg_WstotNum.push_back(eFEXegvalue_tem["WstotNum"]);
    m_eg_WstotDen.push_back(eFEXegvalue_tem["WstotDen"]);
    m_eg_RetaNum.push_back(eFEXegvalue_tem["RetaNum"]);
    m_eg_RetaDen.push_back(eFEXegvalue_tem["RetaDen"]);
    m_eg_RhadNum.push_back(eFEXegvalue_tem["RhadNum"]);
    m_eg_RhadDen.push_back(eFEXegvalue_tem["RhadDen"]);
    m_eg_haveseed.push_back(eFEXegvalue_tem["haveseed"]);
    m_eg_ET.push_back(eFEXegvalue_tem["ET"]);
    m_eg_eta.push_back(eFEXegvalue_tem["eta"]);
    m_eg_phi.push_back(eFEXegvalue_tem["phi"]);
    m_em.push_back(eFEXegvalue_tem["em"]);
    m_had.push_back(eFEXegvalue_tem["had"]);
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXNtupleWriter::loadTruthElectron() {
  m_truth_e_eta.clear();
  m_truth_e_phi.clear();
  m_truth_e_ET.clear();
  const xAOD::TruthEventContainer* truthEvents;
  CHECK(evtStore()->retrieve( truthEvents, "TruthEvents"));
  for(auto ite : *truthEvents) {
    int nParticle = ite->nTruthParticles();
    for(int i = 0; i < nParticle; i++){
      const xAOD::TruthParticle* each_particle = ite->truthParticle(i);

      // ignore geant4
      if(each_particle->barcode() > 200000) continue;
      // select particles that is not decayed further by the generator
      if(each_particle->status() != 1) continue;
      // select electrons
      if(fabs(each_particle->pdgId()) != 11) continue;
      // select particles from Z
      if(!getMother(each_particle, 23)) continue;

      m_truth_e_ET.push_back(each_particle->p4().Pt());
      m_truth_e_eta.push_back(each_particle->p4().Eta());
      m_truth_e_phi.push_back(each_particle->p4().Phi());
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXNtupleWriter::loadTruthJets() {
  m_truth_jet_eta.clear();
  m_truth_jet_phi.clear();
  m_truth_jet_ET.clear();
  const xAOD::JetContainer* truth_jets;
  StatusCode sc = evtStore()->retrieve( truth_jets, m_jet_container_name);
  if (sc ==  StatusCode::FAILURE){
    m_jet_container_name = "InTimeAntiKt4TruthJets";
    StatusCode sc2 = evtStore()->retrieve( truth_jets, m_jet_container_name);
    if (sc2 ==  StatusCode::FAILURE){
      ATH_MSG_DEBUG("eFEXNtupleWriter::loadTruthJets() Unable to determine truth jet container");
      m_load_truth_jet = false;
      return StatusCode::SUCCESS;
    }
  }
  for (unsigned i=0; i!=truth_jets->size(); i++) {
    const xAOD::Jet* each_jet = (*truth_jets)[i];
    if(each_jet->pt()<10000) continue;
    m_truth_jet_eta.push_back(each_jet->p4().Eta());
    m_truth_jet_phi.push_back(each_jet->p4().Phi());
    m_truth_jet_ET.push_back(each_jet->p4().Et());
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXNtupleWriter::loadTruthTau() {
  m_truth_tauvisible_eta.clear();
  m_truth_tauvisible_phi.clear();
  m_truth_tauvisible_ET.clear();
  const xAOD::TruthEventContainer* truthEvents;
  CHECK( evtStore()->retrieve( truthEvents, "TruthEvents"));
  for (auto ite : *truthEvents) {
    int nParticle = ite->nTruthParticles();
    for(int i = 0; i < nParticle; i++) {
      const xAOD::TruthParticle* each_particle = ite->truthParticle(i);
      // ignore geant4
      if (each_particle->barcode() > 200000) continue;
      // select final state particles and decaying hadrons, muons or taus
      if (each_particle->status() != 1 && each_particle->status() != 2) continue;
      // select tau
      if (fabs(each_particle->pdgId()) != 15) continue;
      std::unique_ptr<TLorentzVector> p4_visible = visibleTauP4(each_particle);

      if (!p4_visible) break;
      m_truth_tauvisible_eta.push_back(p4_visible->Eta());
      m_truth_tauvisible_phi.push_back(p4_visible->Phi());
      m_truth_tauvisible_ET.push_back(p4_visible->Et());
    }
  }
  return StatusCode::SUCCESS;
}

std::unique_ptr<TLorentzVector> LVL1::eFEXNtupleWriter::visibleTauP4(const xAOD::TruthParticle* particle) {
  std::unique_ptr<TLorentzVector> psum(new TLorentzVector(0,0,0,0));
  // ignore documentation particles. Attempt to find the nOutgoingParticles() of a documentation particle 
  // causes crash.
  // It still gives the correct result, but I don't know why I have to do this.
  if (particle->status() == 3) {
    return psum;
  }
  const xAOD::TruthVertex* decay_vertex = particle->decayVtx();
  decay_vertex->nOutgoingParticles();
  for(uint i=0; i < decay_vertex->nOutgoingParticles(); i++) {
    const xAOD::TruthParticle* each_particle = decay_vertex->outgoingParticle(i);
    int pid = fabs(each_particle->pdgId());
    // particle that is not decayed further by the generator
    if (each_particle->status() == 1) {
      // ignore neutrinos
      if (pid ==  12 || pid ==  14 || pid ==  16) continue;
      // ignore leptonic decay events
      if (pid ==  11 || pid ==  13) return std::unique_ptr<TLorentzVector>(nullptr);
      (*psum) += each_particle->p4();
    }
    else{
      std::unique_ptr<TLorentzVector> p4_tem = visibleTauP4(each_particle);
      if (!p4_tem) return std::unique_ptr<TLorentzVector>(nullptr);
      (*psum) += (*p4_tem);
    }
  }
  return psum;
}

std::unique_ptr<TLorentzVector> LVL1::eFEXNtupleWriter::invisibleTauP4(const xAOD::TruthParticle* particle) {
  std::unique_ptr<TLorentzVector> psum(new TLorentzVector(0,0,0,0));
  // ignore documentation particles. Attempt to find the nOutgoingParticles() of a documentation particle 
  // causes crash.
  // It still gives the correct result, but I don't know why I have to do this.
  if (particle->status() == 3) {
    return psum;
  }
  const xAOD::TruthVertex* decay_vertex = particle->decayVtx();
  for (uint i=0; i < decay_vertex->nOutgoingParticles(); i++) {
    const xAOD::TruthParticle* each_particle = decay_vertex->outgoingParticle(i);
    int pid = fabs(each_particle->pdgId());
    // particle that is not decayed further by the generator
    if (each_particle->status() == 1) {
      // ignore leptonic decay events
      if (pid ==  11 || pid ==  13) return std::unique_ptr<TLorentzVector>(nullptr);
      // select neutrinos
      if (pid ==  12 || pid ==  14 || pid ==  16) (*psum) += each_particle->p4();
    }
    else {
      std::unique_ptr<TLorentzVector> p4_tem = invisibleTauP4(each_particle);
      if (!p4_tem) return std::unique_ptr<TLorentzVector>(nullptr);
      (*psum) += (*p4_tem);
    }
  }
  return psum;
}

const xAOD::TruthParticle* LVL1::eFEXNtupleWriter::getMother(const xAOD::TruthParticle* particle, int motherPid) {
   const xAOD::TruthVertex* productionVector = particle->prodVtx();
   if (!productionVector) return NULL;
   for (long unsigned int i = 0; i < productionVector->nIncomingParticles(); i++) {
      const xAOD::TruthParticle* mother = productionVector->incomingParticle(i);
      if (mother->pdgId()==motherPid) return mother;
      const xAOD::TruthParticle* grandmother = getMother(mother, motherPid);
      if (grandmother) return grandmother;
   }
   return NULL;
}
