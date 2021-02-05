/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXNtupleWriter.cxx  - copied from eFEXNtupleWrite by Tong Qui
//                              -------------------
//     begin                : 12 12 2020
//     email                : varsiha.sothilingam@cern.ch
//  **************************************************************************

#include <L1CaloFEXSim/jFEXNtupleWriter.h>
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/jFEXSmallRJetAlgo.h"
#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"
//#include "L1CaloFEXSim/jFEXtauAlgo.h"
//#include "L1CaloFEXSim/jFEXtauTOB.h"
#include "L1CaloFEXSim/jFEXOutputCollection.h"
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


LVL1::jFEXNtupleWriter::jFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator) { }

LVL1::jFEXNtupleWriter::~jFEXNtupleWriter() {

}

StatusCode LVL1::jFEXNtupleWriter::initialize () {
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
  CHECK( histSvc.retrieve() );
  m_myTree = new TTree("data","data");
  CHECK( histSvc->regTree("/ANALYSIS/data",m_myTree) );
  
  m_load_truth_jet = false;

  m_myTree->Branch ("truth_smallRJet_eta",  &m_truth_smallRJet_eta);
  m_myTree->Branch ("truth_smallRJet_phi",  &m_truth_smallRJet_phi);
  m_myTree->Branch ("truth_smallRJet_ET",  &m_truth_smallRJet_ET);

  if (m_load_truth_jet){
    m_myTree->Branch ("truth_jet_eta",  &m_truth_jet_eta);
    m_myTree->Branch ("truth_jet_phi",  &m_truth_jet_phi);
    m_myTree->Branch ("truth_jet_ET",  &m_truth_jet_ET);
  }

  m_myTree->Branch ("em",  &m_em);
  m_myTree->Branch ("had",  &m_had);
  m_myTree->Branch ("TT Et", &m_TT_Et);    

  m_myTree->Branch ("smallRJet_eta",  &m_smallRJet_eta);
  m_myTree->Branch ("smallRJet_phi",  &m_smallRJet_phi);
  m_myTree->Branch ("smallRJet_ET",  &m_smallRJet_ET);
  m_myTree->Branch("smallRJet", &m_smallRJet_Sat);
  m_myTree->Branch ("smallRJet_nTOBs",  &m_smallRJet_nTOBs, "nTOBs");
  m_myTree->Branch ("smallRJet_isCentralTowerSeed",  &m_smallRJet_isCentralTowerSeed);
  
  m_myTree->Branch("smallRJetTOB_eta", &m_smallRJetTOB_eta);
  m_myTree->Branch("smallRJetTOB_phi", &m_smallRJetTOB_phi);
  m_myTree->Branch("smallRJetTOB_ET", &m_smallRJetTOB_ET);

  m_myTree->Branch ("largeRJet_ET",  &m_largeRJet_ET);
  m_myTree->Branch ("largeRJet_nTOBs",  &m_largeRJet_nTOBs);

  m_myTree->Branch("largeRJetTOB_eta",  &m_largeRJetTOB_eta);
  m_myTree->Branch ("largeRJetTOB_phi", &m_largeRJetTOB_phi);
  m_myTree->Branch ("largeRJetTOB_ET", &m_largeRJetTOB_ET);

  return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::execute () {
  //ATH_MSG_DEBUG("==== jFEXNtupleWriter ============ execute()");
  ServiceHandle<StoreGateSvc> evtStore("StoreGateSvc/StoreGateSvc",  "arbitrary");
  CHECK(evtStore.retrieve() );

  m_jFEXOutputCollection = new jFEXOutputCollection();
 // m_jFEXOutputCollection = std::shared_ptr<jFEXOutputCollection>(new jFEXOutputCollection());
 // m_jFEXOutputCollection = std::make_shared<jFEXOutputCollection>();
  //m_jFEXOutputCollection = std::make_shared<jFEXOutputCollection>();
  CHECK(evtStore->retrieve(m_jFEXOutputCollection, "jFEXOutputCollection"));

  CHECK(loadsmallRJetAlgoVariables());
  CHECK(loadlargeRJetAlgoVariables());
//CHECK()


 // CHECK(loadegAlgoVariables());
//  CHECK(loadTruthTau());
//  if (m_load_truth_jet){
//    CHECK(loadTruthJets());
//  }

  m_myTree->Fill();
  m_jFEXOutputCollection->clear();
  return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::finalize () {
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::loadsmallRJetAlgoVariables() {
  m_smallRJet_eta.clear();
  m_smallRJet_phi.clear();
  m_smallRJet_isCentralTowerSeed.clear();
  m_smallRJet_ET.clear();
  m_smallRJetTOB_eta.clear();
  m_smallRJetTOB_phi.clear();
  m_smallRJetTOB_ET.clear();

  for (int i = 0; i < m_jFEXOutputCollection->size(); i++)
  {
    m_smallRJet_isCentralTowerSeed.push_back((*(m_jFEXOutputCollection->get_smallRJet(i)))["smallRJet_isCentralTowerSeed"]);
    m_smallRJet_phi.push_back((*(m_jFEXOutputCollection->get_smallRJet(i)))["smallRJet_phi"]);
    m_smallRJet_eta.push_back((*(m_jFEXOutputCollection->get_smallRJet(i)))["smallRJet_eta"]);
    m_smallRJet_ET.push_back((*(m_jFEXOutputCollection->get_smallRJet(i)))["smallRJet_ET"]);
    m_smallRJetTOB_eta.push_back((*(m_jFEXOutputCollection->get_smallRJet(i)))["smallRJetTOB_eta"]);
    m_smallRJetTOB_phi.push_back((*(m_jFEXOutputCollection->get_smallRJet(i)))["smallRJetTOB_phi"]);
    m_smallRJetTOB_ET.push_back((*(m_jFEXOutputCollection->get_smallRJet(i)))["smallRJetTOB_ET"]);
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::loadlargeRJetAlgoVariables() {
  m_largeRJet_ET.clear();
  m_largeRJet_nTOBs.clear();
  m_largeRJetTOB_eta.clear();
  m_largeRJetTOB_phi.clear();
  m_largeRJetTOB_ET.clear();

  for (int i = 0; i < m_jFEXOutputCollection->size(); i++)
  {
    m_largeRJet_ET.push_back((*(m_jFEXOutputCollection->get_largeRJet(i)))["largeRJet_ET"]);
    m_largeRJet_nTOBs.push_back((*(m_jFEXOutputCollection->get_largeRJet(i)))["largeRJet_nTOBs"]); 
    m_largeRJetTOB_eta.push_back((*(m_jFEXOutputCollection->get_largeRJet(i)))["largeRJetTOB_eta"]);
    m_largeRJetTOB_phi.push_back((*(m_jFEXOutputCollection->get_largeRJet(i)))["largeRJetTOB_phi"]);
    m_largeRJetTOB_ET.push_back((*(m_jFEXOutputCollection->get_largeRJet(i)))["largeRJetTOB_ET"]);

  }
  return StatusCode::SUCCESS;
}




/*
StatusCode LVL1::jFEXNtupleWriter::loadTruthElectron() {
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
*/
/*
StatusCode LVL1::jFEXNtupleWriter::loadTruthJets() {
  m_truth_jet_eta.clear();
  m_truth_jet_phi.clear();
  m_truth_jet_ET.clear();
  const xAOD::JetContainer* truth_jets;
  StatusCode sc = evtStore()->retrieve( truth_jets, m_jet_container_name);
  if (sc ==  StatusCode::FAILURE){
    m_jet_container_name = "InTimeAntiKt4TruthJets";
    StatusCode sc2 = evtStore()->retrieve( truth_jets, m_jet_container_name);
    if (sc2 ==  StatusCode::FAILURE){
      ATH_MSG_DEBUG("jFEXNtupleWriter::loadTruthJets() Unable to determine truth jet container");
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
*/
/*StatusCode LVL1::jFEXNtupleWriter::loadTruthTau() {
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
*/
/*
std::unique_ptr<TLorentzVector> LVL1::jFEXNtupleWriter::visibleTauP4(const xAOD::TruthParticle* particle) {
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
*/
/*
std::unique_ptr<TLorentzVector> LVL1::jFEXNtupleWriter::invisibleTauP4(const xAOD::TruthParticle* particle) {
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
*/
/*
const xAOD::TruthParticle* LVL1::jFEXNtupleWriter::getMother(const xAOD::TruthParticle* particle, int motherPid) {
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
*/
