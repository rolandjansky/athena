/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCHitAnalysis.h"

// Section of includes for CSC of the Muon Spectrometer tests
#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <stdio.h>

CSCHitAnalysis::CSCHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_h_hits_x(0)
   , m_h_hits_y(0)
   , m_h_hits_z(0)
   , m_h_hits_r(0)
   , m_h_xy(0)
   , m_h_zr(0)
   , m_h_hits_eta(0)
   , m_h_hits_phi(0)
   , m_h_hits_sx(0)
   , m_h_hits_sy(0)
   , m_h_hits_sz(0)
   , m_h_hits_ex(0)
   , m_h_hits_ey(0)
   , m_h_hits_ez(0)   
   , m_h_hits_time(0)
   , m_h_hits_edep(0)
   , m_h_hits_kine(0)
   , m_hits_x(0)
   , m_hits_y(0)
   , m_hits_z(0)
   , m_hits_r(0)
   , m_hits_eta(0)
   , m_hits_phi(0)
   , m_hits_start_x(0)
   , m_hits_start_y(0)
   , m_hits_start_z(0)
   , m_hits_end_x(0)
   , m_hits_end_y(0)
   , m_hits_end_z(0)   
   , m_hits_time(0)
   , m_hits_edep(0)
   , m_hits_kine(0)
     
   , m_tree(0)
   , m_ntupleFileName("/CSCHitAnalysis/")
   , m_path("/CSCHitAnalysis/")
   , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path); 
}


StatusCode CSCHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing CSCHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  /** Histograms**/
  m_h_hits_x = new TH1D("h_csc_hits_x","hits_x", 100,-2000, 2000);
  m_h_hits_x->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_x->GetName(), m_h_hits_x));

  m_h_hits_y = new TH1D("h_csc_hits_y", "hits_y", 100,-2000,2000);
  m_h_hits_y->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_y->GetName(), m_h_hits_y));

  m_h_hits_z = new TH1D("h_csc_hits_z", "hits_z", 100,-10000,10000);
  m_h_hits_z->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_z->GetName(), m_h_hits_z));

  m_h_hits_r = new TH1D("h_csc_hits_r", "hits_r", 100,500,2500);
  m_h_hits_r->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_r->GetName(), m_h_hits_r));

  m_h_xy = new TH2D("h_csc_xy", "xy", 100,-2000.,2000.,100, -2000., 2000.);
  m_h_xy->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_xy->GetName(), m_h_xy));

  m_h_zr = new TH2D("h_csc_zr", "zr", 100,-10000.,10000.,100, 500., 2500.);
  m_h_zr->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_zr->GetName(), m_h_zr));

  m_h_hits_eta = new TH1D("h_csc_hits_eta", "hits_eta", 100,-3.0,3.0);
  m_h_hits_eta->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_eta->GetName(), m_h_hits_eta));

  m_h_hits_phi = new TH1D("h_csc_hits_phi", "hits_phi", 100,-3.2,3.2);
  m_h_hits_phi->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_phi->GetName(), m_h_hits_phi));

  m_h_hits_sx = new TH1D("h_csc_hits_sx","hits_sx", 100,-10, 10);
  m_h_hits_sx->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_sx->GetName(), m_h_hits_sx));

  m_h_hits_sy = new TH1D("h_csc_hits_sy", "hits_sy", 100,-500,500);
  m_h_hits_sy->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_sy->GetName(), m_h_hits_sy));

  m_h_hits_sz = new TH1D("h_csc_hits_sz", "hits_sz", 100,-1000,1000);
  m_h_hits_sz->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_sz->GetName(), m_h_hits_sz));

  m_h_hits_ex = new TH1D("h_csc_hits_ex","hits_ex", 100,-10, 10);
  m_h_hits_ex->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_ex->GetName(), m_h_hits_ex));

  m_h_hits_ey = new TH1D("h_csc_hits_ey", "hits_ey", 100,-500,500);
  m_h_hits_ey->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_ey->GetName(), m_h_hits_ey));

  m_h_hits_ez = new TH1D("h_csc_hits_ez", "hits_ez", 100,-1000,1000);
  m_h_hits_ez->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_ez->GetName(), m_h_hits_ez));

  m_h_hits_time = new TH1D("h_csc_hits_time","hits_time", 100,20, 40);
  m_h_hits_time->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_time->GetName(), m_h_hits_time));

  m_h_hits_edep = new TH1D("h_csc_hits_edep", "hits_edep", 100,0,0.1);
  m_h_hits_edep->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_edep->GetName(), m_h_hits_edep));

  m_h_hits_kine = new TH1D("h_csc_hits_kine", "hits_kine", 100,0,3000);
  m_h_hits_kine->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_kine->GetName(), m_h_hits_kine));

  /** now add branches and leaves to the tree */
  m_tree = new TTree("CSC", "CSC");
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/";
  CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  
  if (m_tree){
    m_tree->Branch("x", &m_hits_x);
    m_tree->Branch("y", &m_hits_y);
    m_tree->Branch("z", &m_hits_z);
    m_tree->Branch("r", &m_hits_r);
    m_tree->Branch("eta", &m_hits_eta);
    m_tree->Branch("phi", &m_hits_phi);
    m_tree->Branch("startX", &m_hits_start_x);
    m_tree->Branch("startY", &m_hits_start_y);
    m_tree->Branch("startZ", &m_hits_start_z);
    m_tree->Branch("endX", &m_hits_end_x);
    m_tree->Branch("endY", &m_hits_end_y);
    m_tree->Branch("endZ", &m_hits_end_z);
    m_tree->Branch("time", &m_hits_time);
    m_tree->Branch("edep", &m_hits_edep);
    m_tree->Branch("kine", &m_hits_kine);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }
  
  return StatusCode::SUCCESS;
}


StatusCode CSCHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In CSCHitAnalysis::execute()" );
  
  m_hits_x->clear();
  m_hits_y->clear();
  m_hits_z->clear();
  m_hits_r->clear();
  m_hits_eta->clear();
  m_hits_phi->clear();
  m_hits_start_x->clear();
  m_hits_start_y->clear();
  m_hits_start_z->clear();
  m_hits_end_x->clear();
  m_hits_end_y->clear();
  m_hits_end_z->clear();
  m_hits_time->clear();
  m_hits_edep->clear();
  m_hits_kine->clear();

  const DataHandle<CSCSimHitCollection> csc_container;
  if (evtStore()->retrieve(csc_container, "CSC_Hits") == StatusCode::SUCCESS) {
    for (CSCSimHitCollection::const_iterator i_hit = csc_container->begin(); 
	i_hit != csc_container->end(); ++i_hit) {
      //CSCSimHitCollection::const_iterator i_hit;
      //for(auto i_hit : *csc_container){
      GeoCSCHit ghit(*i_hit);
      if (!ghit) continue;
      
      Amg::Vector3D p = ghit.getGlobalPosition();
      m_h_hits_x->Fill(p.x());
      m_h_hits_y->Fill(p.y());
      m_h_hits_z->Fill(p.z());
      m_h_hits_r->Fill(p.perp());
      m_h_xy->Fill(p.x(), p.y());
      m_h_zr->Fill(p.z(), p.perp());
      m_h_hits_eta->Fill(p.eta());
      m_h_hits_phi->Fill(p.phi());
      m_h_hits_sx->Fill((*i_hit).getHitStart().x());
      m_h_hits_sy->Fill((*i_hit).getHitStart().y());
      m_h_hits_sz->Fill((*i_hit).getHitStart().z());
      m_h_hits_ex->Fill((*i_hit).getHitEnd().x());
      m_h_hits_ey->Fill((*i_hit).getHitEnd().y());
      m_h_hits_ez->Fill((*i_hit).getHitEnd().z());
      m_h_hits_edep->Fill((*i_hit).energyDeposit());
      m_h_hits_time->Fill((*i_hit).globalTime());
      m_h_hits_kine->Fill((*i_hit).kineticEnergy());
      
      m_hits_x->push_back(p.x());
      m_hits_y->push_back(p.y());
      m_hits_z->push_back(p.z());
      m_hits_r->push_back(p.perp());
      m_hits_eta->push_back(p.eta());
      m_hits_phi->push_back(p.phi());
      m_hits_start_x->push_back((*i_hit).getHitStart().x());
      m_hits_start_y->push_back((*i_hit).getHitStart().y());
      m_hits_start_z->push_back((*i_hit).getHitStart().z());
      m_hits_end_x->push_back((*i_hit).getHitEnd().x());
      m_hits_end_y->push_back((*i_hit).getHitEnd().y());
      m_hits_end_z->push_back((*i_hit).getHitEnd().z());
      m_hits_edep->push_back((*i_hit).energyDeposit());
      m_hits_time->push_back((*i_hit).globalTime());
      m_hits_kine->push_back((*i_hit).kineticEnergy());    
    }
  } // End while hits
  
  if (m_tree) m_tree->Fill();
 
  return StatusCode::SUCCESS;
  
}
