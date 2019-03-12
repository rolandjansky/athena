/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//////////////////////////////////////////////////////////////////
// CalibrationNtupleMakerTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CalibrationNtupleMakerTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"

#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Root
#include "TTree.h"
#include "TString.h"
#include "TH1F.h"

#include <vector>

using Gaudi::Units::GeV;

CalibrationNtupleMakerTool::CalibrationNtupleMakerTool(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_treeFolder("/calibration/"),  
  m_treeDescription("Calibration Ntuple"),
  m_truthJetContainerName("MyAntiKt10TruthJets"),
  m_vertexContainerName("PrimaryVertices"),
  m_recoIsoDR(1.5),
  m_recoIsoPtCut(100.*GeV),
  m_trueIsoDR(2.5),
  m_trueIsoPtCut(100.*GeV),  
  m_matchingCut(0.6),  
  m_h_events(nullptr),
  m_index(0),  
  m_etaCalo(0),
  m_etaDetCalo(0),
  m_phiCalo(0),
  m_eCalo(0),
  m_mCalo(0),  
  m_etaCorr(0),
  m_etaDetCorr(0),
  m_phiCorr(0),
  m_eCorr(0),
  m_mCorr(0),              
  m_etaTrue(0),
  m_phiTrue(0),
  m_eTrue(0),
  m_mTrue(0)
  {    
    declareProperty("FolderName" , m_treeFolder);
    declareProperty("Description", m_treeDescription);
    
    // the jets collections to calibrate
    declareProperty("JetCollections"       , m_collectionNames);
    declareProperty("TruthJetContainerName", m_truthJetContainerName);
    declareProperty("VertexContainerName"  , m_vertexContainerName);
    
    declareProperty("RecoIsolationDR"    , m_recoIsoDR);
    declareProperty("RecoIsolationPtCut" , m_recoIsoPtCut);
    declareProperty("TruthIsolationDR"   , m_trueIsoDR);
    declareProperty("TruthIsolationPtCut", m_trueIsoPtCut);
    declareProperty("MatchingCut"        , m_matchingCut);   
  }
  
CalibrationNtupleMakerTool::~CalibrationNtupleMakerTool()
{}
  
StatusCode CalibrationNtupleMakerTool::initialize()
{
  ATH_MSG_INFO( "initialize()" );
  
  if  (bookTree().isFailure()){
    ATH_MSG_FATAL( "Could not book the TTree object" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_evt.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode CalibrationNtupleMakerTool::bookTree()
{
  ATH_MSG_INFO( "bookTree()" );
  
  
  for (auto& name : m_collectionNames) {
    
    // creating the tree for thw jet collection
    TTree * tree = new TTree (name.c_str(), m_treeDescription.c_str());
    
    // add the branches
    tree->Branch("EventWeight"    ,  &m_eventWeight   );
    
    tree->Branch("eta_calo"       ,  &m_etaCalo       );
    tree->Branch("eta_det_calo"   ,  &m_etaDetCalo    );
    tree->Branch("phi_calo"       ,  &m_phiCalo       );
    tree->Branch("E_calo"         ,  &m_eCalo         );
    tree->Branch("m_calo"         ,  &m_mCalo         );
                                                      
    tree->Branch("eta_corr1"      ,  &m_etaCorr       );
    tree->Branch("eta_det_corr1"  ,  &m_etaDetCorr    );
    tree->Branch("phi_corr1"      ,  &m_phiCorr       );
    tree->Branch("E_corr1"        ,  &m_eCorr         );
    tree->Branch("m_corr1"        ,  &m_mCorr         );
                                                      
    tree->Branch("eta_true"       ,  &m_etaTrue       );
    tree->Branch("phi_true"       ,  &m_phiTrue       );
    tree->Branch("E_true"         ,  &m_eTrue         );
    tree->Branch("m_true"         ,  &m_mTrue         );
                                                      
    tree->Branch("index"          ,  &m_index         );
    tree->Branch("mu"             ,  &m_mu            );
    tree->Branch("NPV"            ,  &m_npv           );
    
    
    m_trees.insert(std::pair<std::string, TTree*>(name, tree));
    
  }
  
  // now register the Tree
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()) {
    ATH_MSG_ERROR( "initialize() Could not find Hist Service!" );
    return StatusCode::FAILURE;
  }
  
  if (tHistSvc) {
    for (auto name : m_collectionNames) {
      if((tHistSvc->regTree(m_treeFolder+name, m_trees.at(name))).isFailure()) {
	ATH_MSG_ERROR( "initialize() Could not register the validation Tree!" );
	return StatusCode::FAILURE;
      }
    }
  }
  
  // now register the Event histogram
  m_h_events = new TH1F("h_events","total events", 10, 0, 10);
  
  if (tHistSvc and (tHistSvc->regHist(m_treeFolder+m_h_events->GetName(), m_h_events)).isFailure()) {
    ATH_MSG_ERROR( "Can not register histogram" <<  m_h_events->GetName() );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Calibration Tree booked and registered successfully!");
  
  return StatusCode::SUCCESS;
  
}

StatusCode CalibrationNtupleMakerTool::execute()
{
  m_h_events->Fill(0);
  
  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  if(!evt.isValid()) {
    ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  } 
  float ev_weight = evt->mcEventWeight();

  const auto truths = getContainer<xAOD::JetContainer>(m_truthJetContainerName);  
  if (not truths) return StatusCode::FAILURE;
  
  const auto vertices = getContainer<xAOD::VertexContainer>(m_vertexContainerName);
  if (not vertices) return StatusCode::FAILURE;
  
  // get mu
  float mu= evt->averageInteractionsPerCrossing();
  
  //get NPV
  float npv = 0.;
  
  for (const auto& vertex : *vertices) {
    if (vertex->nTrackParticles()>=2)
      npv++;
  }
  
  for (auto& name : m_collectionNames) {
    const auto jets = getContainer<xAOD::JetContainer>(name);

    m_etaCalo       ->clear();
    m_etaDetCalo    ->clear();
    m_phiCalo       ->clear();
    m_eCalo         ->clear();
    m_mCalo         ->clear();
    m_etaCorr       ->clear();
    m_etaDetCorr    ->clear();
    m_phiCorr       ->clear();
    m_eCorr         ->clear();
    m_mCorr         ->clear();
    m_etaTrue       ->clear();
    m_phiTrue       ->clear();
    m_eTrue         ->clear();
    m_mTrue         ->clear();
    m_index         ->clear();
    
    for (const auto& truth: *truths) {

      // here we match to the reco
      int index = 0;
      std::vector<const xAOD::Jet*> matched = {};
      
      int Nmatches = Matched(truth, jets, matched, index);
      
      // skip truth jets that don't match any reco jets
      if (Nmatches==0) continue; 

      // skip the jets that are not isolated
      if ( m_recoIsoDR > 0 ) {
	double DRminReco = DRmin(matched.at(0),jets,m_recoIsoPtCut);
	if ( DRminReco < m_recoIsoDR ) continue;
      }
      
      if ( m_trueIsoDR > 0 ) {
	double DRminTruth = DRmin(truth,truths,m_trueIsoPtCut);
	if ( DRminTruth < m_trueIsoDR ) continue;
      }
      
      //Storing variables
      m_etaTrue->push_back(truth->eta());
      m_phiTrue->push_back(truth->phi());
      m_mTrue->push_back(truth->m()/GeV);
      m_eTrue->push_back(truth->e()/GeV);
      
      m_etaCalo->push_back(jets->at(index)->eta());
      m_phiCalo->push_back(jets->at(index)->phi());
      m_mCalo->push_back(jets->at(index)->m()/GeV);
      m_eCalo->push_back(jets->at(index)->e()/GeV);
      
      float detectorEta = DetectorEta(jets->at(index));
      m_etaDetCalo->push_back(detectorEta);
            
      m_etaCorr->push_back(jets->at(index)->eta());
      m_phiCorr->push_back(jets->at(index)->phi());
      m_mCorr->push_back(jets->at(index)->m()/GeV);
      m_eCorr->push_back(jets->at(index)->e()/GeV);
      m_etaDetCorr->push_back(detectorEta);
      
      m_index->push_back(index);
      
    }
    
    m_eventWeight = ev_weight;
    m_mu          = mu;
    m_npv         = npv;
    
    m_trees.at(name)->Fill();
    
  }
  
  return StatusCode::SUCCESS;
  
}

int CalibrationNtupleMakerTool::Matched(const xAOD::Jet* truth, const xAOD::JetContainer* jets, std::vector<const xAOD::Jet*>& matched, int& index) {
  
  int Nmatches = 0;
  double drmin = 999.;
  int Min_index=-1;

  for (unsigned int ind = 0; ind < jets->size(); ind++) {  
    double dr = truth->p4().DeltaR(jets->at(ind)->p4());
    if (dr < m_matchingCut) ++Nmatches;
    //find minimum:
    if (dr < drmin) {
      drmin     = dr;
      Min_index = ind;
    }
  }
  
  if (drmin<m_matchingCut) {
    matched.push_back(jets->at(Min_index));
    index   = Min_index;
  }
  
  return Nmatches;
}

double CalibrationNtupleMakerTool::DRmin(const xAOD::Jet* myjet, const xAOD::JetContainer* jets, double PtMin) {

  double DRmin=9999;
  for (const auto& jet : *jets) {
    if (PtMin>0. and jet->pt()<PtMin) continue;
    double Dr = myjet->p4().DeltaR(jet->p4());
    if (Dr>0.0001 and Dr<DRmin) 
      DRmin=Dr;
  }
  return DRmin;
}

float CalibrationNtupleMakerTool::DetectorEta(const xAOD::Jet* jet) {
  
  xAOD::IParticle::FourMom_t corrP4(0,0,0,0);
      
  const auto& partLinks = jet->constituentLinks();
  for (const xAOD::IParticleLink& link : partLinks) {
    
    if (not link.isValid()) {
      ATH_MSG_WARNING("Got an invalid element link. Returning jet eta...");
      return jet->eta();
    }
    
    const xAOD::TrackCaloCluster* tcc = dynamic_cast<const xAOD::TrackCaloCluster*>(*link);
    
    static SG::AuxElement::Accessor< float > acc_detEta( "DetectorEta" );
    float det_eta = tcc->eta();
    
    if (acc_detEta.isAvailable(*tcc)) {
      det_eta = acc_detEta(*tcc);      
    } else
      ATH_MSG_WARNING("DetectorEta decoration not found for TCCs! Using eta...");
    
    double pt = tcc->p4().P()/cosh(det_eta);
    
    xAOD::IParticle::FourMom_t p4CorrCl;
    p4CorrCl.SetPtEtaPhiE(pt, det_eta, tcc->p4().Phi(), tcc->p4().E());
    if(tcc->p4().E() < 0.) p4CorrCl*=-1.;
    corrP4 += p4CorrCl;  
  }
    
  return corrP4.Eta();
}

StatusCode CalibrationNtupleMakerTool::finalize()
{
  ATH_MSG_INFO( "finalize()" );
  
  delete  m_index;
  
  delete  m_etaCalo;
  delete  m_etaDetCalo;
  delete  m_phiCalo;
  delete  m_eCalo;
  delete  m_mCalo;
  
  delete  m_etaCorr;
  delete  m_etaDetCorr;
  delete  m_phiCorr;
  delete  m_eCorr;
  delete  m_mCorr;
  
  delete  m_etaTrue;
  delete  m_phiTrue;
  delete  m_eTrue;
  delete  m_mTrue;
  
  return StatusCode::SUCCESS;
  
}
