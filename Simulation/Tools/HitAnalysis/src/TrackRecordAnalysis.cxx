/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackRecordAnalysis.h"

// Section of includes for TrackRecord tests
#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <stdio.h>

TrackRecordAnalysis::TrackRecordAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_h_hits_x(0)
   , m_h_hits_y(0)
   , m_h_hits_z(0)
   , m_h_hits_r(0)
   , m_h_xy(0)
   , m_h_zr(0)
   , m_h_hits_eta(0)
   , m_h_hits_phi(0)
   , m_h_hits_px(0)
   , m_h_hits_py(0)
   , m_h_hits_pz(0)
   , m_h_hits_pt(0)
   , m_h_time(0)
   , m_h_edep(0)
   , m_h_pdg(0)
   , m_x(0)
   , m_y(0)
   , m_z(0)
   , m_r(0)
   , m_eta(0)
   , m_phi(0)
   , m_px(0)
   , m_py(0)
   , m_pz(0)
   , m_pt(0)
   , m_time(0)
   , m_edep(0)
   , m_pdg(0)
     
   , m_collection("CaloEntryLayer")
     
   , m_tree(0)
   , m_ntupleFileName("/TrackRecordAnalysis/")
   , m_path("/TrackRecordAnalysis/")
   , m_thistSvc("THistSvc", name)
{
  declareProperty("CollectionName", m_collection="CaloEntryLayer"); 
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path); 
}


StatusCode TrackRecordAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing TrackRecordAnalysis" );

  std::string detName("CaloEntry");
  std::string ntupName("TrackRecordCaloEntry");
  
  if (m_collection == "CaloEntryLayer") {
    detName="CaloEntry";
    ntupName="TrackRecordCaloEntry";
  }
  else if (m_collection == "MuonEntryLayer") {
    detName="MuonEntry";
    ntupName="TrackRecordMuonEntry";
  }
  else if (m_collection == "MuonExitLayer") {
    detName="MuonExit";
    ntupName="TrackRecordMuonExit";
  }
  else {
    ATH_MSG_ERROR("TrackRecordAnalysis for "<< name() << "not supported !!! \n");
    return StatusCode::FAILURE;
  }
  
  std::cout << "Name " << name() << std::endl;
  
  //Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  float x_down = -5000;
  float x_up =  5000;
  float radius =  4500;
  float eta_down =  -5.8;
  float eta_up = 5.8;
  float z_down = -7000;
  float z_up = 7000;
  if (detName == "CaloEntry") {
    x_down = -1180;
    x_up =  1180;
    radius = 1200;
    eta_down = - 5.6;
    eta_up = 5.6;
    z_down = -3700;
    z_up = 3700;
  }

  /** Histograms**/
  m_h_hits_x = new TH1D((detName+"_x").c_str(),"hits_x", 100,x_down, x_up);
  m_h_hits_x->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_x->GetName(), m_h_hits_x));

  m_h_hits_y = new TH1D((detName+"_y").c_str(), "hits_y", 100,x_down,x_up);
  m_h_hits_y->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_y->GetName(), m_h_hits_y));

  m_h_hits_z = new TH1D((detName+"_z").c_str(), "hits_z", 100,z_down,z_up);
  m_h_hits_z->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_z->GetName(), m_h_hits_z));

  m_h_hits_r = new TH1D((detName+"_r").c_str(), "hits_r", 100,0,radius);
  m_h_hits_r->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_r->GetName(), m_h_hits_r));

  m_h_xy = new TH2D((detName+"_xy").c_str(), "xy", 100,x_down,x_up,100, x_down, x_up);
  m_h_xy->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_xy->GetName(), m_h_xy));

  m_h_zr = new TH2D((detName+"_zr").c_str(), "zr", 100,z_down,z_up,100, 0., radius);
  m_h_zr->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_zr->GetName(), m_h_zr));

  m_h_hits_eta = new TH1D((detName+"_eta").c_str(), "hits_eta", 100,eta_down,eta_up);
  m_h_hits_eta->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_eta->GetName(), m_h_hits_eta));

  m_h_hits_phi = new TH1D((detName+"_phi").c_str(), "hits_phi", 100,-3.1416,3.1416);
  m_h_hits_phi->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_phi->GetName(), m_h_hits_phi));

  m_h_hits_px = new TH1D((detName+"_px").c_str(),"Px", 100, -1500, 1500);
  m_h_hits_px->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_px->GetName(), m_h_hits_px));

  m_h_hits_py = new TH1D((detName+"_py").c_str(), "Py", 100,-1500,1500);
  m_h_hits_py->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_py->GetName(), m_h_hits_py));

  m_h_hits_pz = new TH1D((detName+"_pz").c_str(), "Pz", 100,-1500,1500);
  m_h_hits_pz->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_pz->GetName(), m_h_hits_pz));

  m_h_hits_pt = new TH1D((detName+"_pt").c_str(), "hits_pt", 100,0,2500);
  m_h_hits_pt->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_pt->GetName(), m_h_hits_pt));

  m_h_time = new TH1D((detName+"_time").c_str(),"time", 100,0, 250);
  m_h_time->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_time->GetName(), m_h_time));

  m_h_edep = new TH1D((detName+"_edep").c_str(), "energy", 100,0,7500);
  m_h_edep->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_edep->GetName(), m_h_edep));

  m_h_pdg = new TH1D((detName+"_pdg").c_str(), "pdg", 100,-1000,1000);
  m_h_pdg->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_pdg->GetName(), m_h_pdg));

  /** now add branches and leaves to the tree */
  m_tree = new TTree(ntupName.c_str(), ntupName.c_str());
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/" + detName;
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));
    
  if (m_tree){
    m_tree->Branch("x", &m_x);
    m_tree->Branch("y", &m_y);
    m_tree->Branch("z", &m_z);
    m_tree->Branch("r", &m_r);
    m_tree->Branch("eta", &m_eta);
    m_tree->Branch("phi", &m_phi);
    m_tree->Branch("px", &m_px);
    m_tree->Branch("py", &m_py);
    m_tree->Branch("pz", &m_pz);
    m_tree->Branch("pt", &m_pt);
    m_tree->Branch("time", &m_time);
    m_tree->Branch("energy", &m_edep);
    m_tree->Branch("pdg", &m_pdg);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  return StatusCode::SUCCESS;
}		 


StatusCode TrackRecordAnalysis::execute() {
  ATH_MSG_DEBUG( "In TrackRecordAnalysis::execute()" );

  m_x->clear();
  m_y->clear();
  m_z->clear();
  m_r->clear();
  m_eta->clear();
  m_phi->clear();
  m_px->clear();
  m_py->clear();
  m_pz->clear();
  m_pt->clear();
  m_time->clear();
  m_edep->clear();
  m_pdg->clear();
    
  const DataHandle<TrackRecordCollection> TRcoll;
  if (evtStore()->retrieve(TRcoll, m_collection ) == StatusCode::SUCCESS) {
    for (TrackRecordCollection::const_iterator track = TRcoll->begin(); track != TRcoll->end(); ++track) {
      
      CLHEP::Hep3Vector p =(*track).GetPosition();
      m_h_hits_x->Fill(p.x());
      m_h_hits_y->Fill(p.y());
      m_h_hits_z->Fill(p.z());
      m_h_hits_r->Fill(p.perp());
      m_h_xy->Fill(p.x(), p.y());
      m_h_zr->Fill(p.z(),p.perp());
      m_h_hits_eta->Fill(p.eta());
      m_h_hits_phi->Fill(p.phi());
      
      CLHEP::Hep3Vector mom = (*track).GetMomentum();
      m_h_hits_px->Fill(mom.x());
      m_h_hits_py->Fill(mom.y());
      m_h_hits_pz->Fill(mom.z());
      m_h_hits_pt->Fill(mom.perp());
      m_h_edep->Fill((*track).GetEnergy());
      m_h_time->Fill((*track).GetTime());
      m_h_pdg->Fill((*track).GetPDGCode());
  
      m_x->push_back(p.x());
      m_y->push_back(p.y());
      m_z->push_back(p.z());
      m_r->push_back(p.perp());
      m_eta->push_back(p.eta());
      m_phi->push_back(p.phi());
      m_px->push_back(mom.x());
      m_py->push_back(mom.y());
      m_pz->push_back(mom.z());
      m_pt->push_back(mom.perp());
      m_edep->push_back((*track).GetEnergy());
      m_time->push_back((*track).GetTime());
      m_pdg->push_back((*track).GetPDGCode());
    }
  } // End while hits 
 
  if (m_tree) m_tree->Fill();
 
  return StatusCode::SUCCESS;
}
