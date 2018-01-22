/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHitAnalysis.h"

// Section of includes for Truth tests
#include "HepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

TruthHitAnalysis::TruthHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_h_n_vert(0)
   , m_h_n_part(0)
   , m_h_n_vert_prim(0)
   , m_h_n_part_prim(0)
   , m_h_n_vert_sec(0)
   , m_h_n_part_sec(0)
   , m_h_vtx_x(0)
   , m_h_vtx_y(0)
   , m_h_vtx_z(0)
   , m_h_vtx_r(0)
   , m_h_vtx_prim_xy(0)
   , m_h_vtx_prim_zr(0)
   , m_h_vtx_sec_xy(0)
   , m_h_vtx_sec_zr(0)
   , m_h_n_generations(0)
   , m_h_truth_px(0)
   , m_h_truth_py(0)
   , m_h_truth_pz(0)
   , m_h_truth_pt(0)
   , m_h_truth_eta(0)
   , m_h_truth_phi(0)
   , m_h_barcode(0)
   , m_h_part_status(0)
   , m_h_part_pdgid(0)
   , m_h_part_pdgid_sec(0)
   , m_h_part_eta(0)
   , m_h_part_phi(0)
   , m_h_part_p(0)
   , m_vtx_x(0)
   , m_vtx_y(0)
   , m_vtx_z(0)
   , m_vtx_r(0)
   , m_vtx_barcode(0)
   , m_truth_px(0)
   , m_truth_py(0)
   , m_truth_pz(0)
   , m_truth_pt(0)
   , m_truth_eta(0)
   , m_truth_phi(0)
   , m_barcode(0)
   , m_status(0)
   , m_pdgid(0)
     
   , m_tree(0)
   , m_ntupleFileName("/TruthHitAnalysis/")
   , m_path("/TruthHitAnalysis/")
   , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path);  
}


StatusCode TruthHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing TruthHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  /** histograms declaration */
  m_h_n_vert = new TH1D("h_n_vert","n_vert", 100,200, 1500);
  m_h_n_vert->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_n_vert->GetName(), m_h_n_vert));

  m_h_n_part = new TH1D("h_n_part","n_part", 100,1000, 10000);
  m_h_n_part->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_n_part->GetName(), m_h_n_part));

  m_h_n_vert_prim = new TH1D("h_n_vert_prim","n_vert prim", 100,0, 1000);
  m_h_n_vert_prim->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_n_vert_prim->GetName(), m_h_n_vert_prim));

  m_h_n_part_prim = new TH1D("h_n_part_prim","n_part prim", 100,200, 1500);
  m_h_n_part_prim->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_n_part_prim->GetName(), m_h_n_part_prim));

  m_h_n_vert_sec = new TH1D("h_n_vert_sec","n_vert sec", 100,0, 1000);
  m_h_n_vert_sec->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_n_vert_sec->GetName(), m_h_n_vert_sec));

  m_h_n_part_sec = new TH1D("h_n_part_sec","n_part sec", 100,0, 5000);
  m_h_n_part_sec->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_n_part_sec->GetName(), m_h_n_part_sec));

  m_h_vtx_x = new TH1D("h_vtx_x","vtx_x", 100,-1300, 1300);
  m_h_vtx_x->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_x->GetName(), m_h_vtx_x));

  m_h_vtx_y = new TH1D("h_vtx_y","vtx_y", 100,-1200, 1200);
  m_h_vtx_y->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_y->GetName(), m_h_vtx_y));

  m_h_vtx_z = new TH1D("h_vtx_z","vtx_z", 100,-5000, 5000);
  m_h_vtx_z->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_z->GetName(), m_h_vtx_z));

  m_h_vtx_r = new TH1D("h_vtx_r","vtx_r", 100,0, 1160);
  m_h_vtx_r->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_r->GetName(), m_h_vtx_r));

  m_h_vtx_prim_xy = new TH2D("h_vtx_prim_xy","vtx_prim_xy", 100,-100, 100, 100,-100, 100);
  m_h_vtx_prim_xy->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_prim_xy->GetName(), m_h_vtx_prim_xy));

  m_h_vtx_prim_zr = new TH2D("h_vtx_prim_zr","vtx_prim_zr", 100,-1500, 1500, 100,0, 150);
  m_h_vtx_prim_zr->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_prim_zr->GetName(), m_h_vtx_prim_zr));

  m_h_vtx_sec_xy = new TH2D("h_vtx_sec_xy","vtx_sec_xy", 100,-1200, 1200, 100,-1200, 1200);
  m_h_vtx_sec_xy->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_sec_xy->GetName(), m_h_vtx_sec_xy));

  m_h_vtx_sec_zr = new TH2D("h_vtx_sec_zr","vtx_sec_zr", 100,-6000, 6000, 100,0, 1160);
  m_h_vtx_sec_zr->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_vtx_sec_zr->GetName(), m_h_vtx_sec_zr));

  m_h_n_generations = new TH1D("h_n_generations","h_generations", 100,0, 25);
  m_h_n_generations->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_n_generations->GetName(), m_h_n_generations));

  m_h_truth_px = new TH1D("h_turht_px","truth_px", 100,0, 4000);
  m_h_truth_px->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_truth_px->GetName(), m_h_truth_px));

  m_h_truth_py = new TH1D("h_turht_py","truth_py", 100,0, 4000);
  m_h_truth_py->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_truth_py->GetName(), m_h_truth_py));

  m_h_truth_pz = new TH1D("h_truth_pz","truth_pz", 100,0, 4000);
  m_h_truth_pz->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_truth_pz->GetName(), m_h_truth_pz));

  m_h_truth_pt = new TH1D("h_truth_pt","truth_pt", 100,0, 4000);
  m_h_truth_pt->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_truth_pt->GetName(), m_h_truth_pt));

  m_h_truth_eta = new TH1D("h_truth_eta","truth_eta", 50,-10, 10);
  m_h_truth_eta->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_truth_eta->GetName(), m_h_truth_eta));

  m_h_truth_phi = new TH1D("h_truth_phi","truth_phi", 25,-3.1416, 3.1416);
  m_h_truth_phi->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_truth_phi->GetName(), m_h_truth_phi));

  m_h_barcode = new TH1D("h_truth_barcode","truth_barcode", 100,0, 300000);
  m_h_barcode->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_barcode->GetName(), m_h_barcode));

  m_h_part_status = new TH1D("h_part_status","part status", 100,0,50);
  m_h_part_status->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_part_status->GetName(), m_h_part_status));

  m_h_part_pdgid = new TH1D("h_part_pdgid","part pdgid", 100,-5000, 5000);
  m_h_part_pdgid->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_part_pdgid->GetName(), m_h_part_pdgid));

  m_h_part_pdgid_sec = new TH1D("h_part_pdgid_sec","part pdgid sec", 100,-5000, 5000);
  m_h_part_pdgid_sec->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_part_pdgid_sec->GetName(), m_h_part_pdgid_sec));

  m_h_part_eta = new TH1D("h_part_eta","part eta", 100,-10, 10);
  m_h_part_eta->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_part_eta->GetName(), m_h_part_eta));

  m_h_part_phi = new TH1D("h_part_phi","part phi", 100,-3.2, 3.2);
  m_h_part_phi->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_part_phi->GetName(), m_h_part_phi));

  m_h_part_p = new TH1D("h_part_p","part p", 100,0, 5000);
  m_h_part_p->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_part_p->GetName(), m_h_part_p));

  /** now add branches and leaves to the tree */
  m_tree = new TTree("Truth", "Truth");
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/";
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));
  
  if (m_tree) {
    m_tree->Branch("vtx_x", &m_vtx_x);
    m_tree->Branch("vtx_y", &m_vtx_y);
    m_tree->Branch("vtx_z", &m_vtx_z);
    m_tree->Branch("vtx_r", &m_vtx_r);
    m_tree->Branch("vtx_barcode", &m_vtx_barcode);
    m_tree->Branch("truth_px", &m_truth_px);
    m_tree->Branch("truth_py", &m_truth_py);
    m_tree->Branch("truth_pz", &m_truth_pz);
    m_tree->Branch("truth_pt", &m_truth_pt);
    m_tree->Branch("truth_eta", &m_truth_eta);
    m_tree->Branch("truth_phi", &m_truth_phi);
    m_tree->Branch("barcode", &m_barcode);
    m_tree->Branch("status", &m_status);
    m_tree->Branch("pdg_id", &m_pdgid); 
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  return StatusCode::SUCCESS;
}


StatusCode TruthHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In TruthHitAnalysis::execute()" );

  m_vtx_x->clear();
  m_vtx_y->clear();
  m_vtx_z->clear();
  m_vtx_r->clear();
  m_vtx_barcode->clear();
  m_truth_px->clear();
  m_truth_py->clear();
  m_truth_pz->clear();
  m_truth_pt->clear();
  m_truth_eta->clear();
  m_truth_phi->clear();
  m_barcode->clear();
  m_status->clear();
  m_pdgid->clear();
  
  const DataHandle<EventInfo> event;
  if (!evtStore()->retrieve(event, "McEventInfo" ).isSuccess()) 
    return StatusCode::FAILURE;
  const DataHandle<McEventCollection> mcCollection;
  if (evtStore()->retrieve(mcCollection,"TruthEvent") == StatusCode::SUCCESS) {
    McEventCollection::const_iterator currentGenEventIter = mcCollection->begin(); 
    if (currentGenEventIter != mcCollection->end()) {
      int nvtx = 0;
      int nvtx_sec=0;
      for (HepMC::GenEvent::vertex_const_iterator vtx=(*currentGenEventIter)->vertices_begin(); vtx!=(*currentGenEventIter)->vertices_end(); ++vtx) {
	double x = (*vtx)->position().x();
	double y = (*vtx)->position().y();
	double z = (*vtx)->position().z();
	double r = sqrt(x*x+y*y);
	m_h_vtx_x->Fill(x);
	m_h_vtx_y->Fill(y);
	m_h_vtx_r->Fill(r);
	m_h_vtx_z->Fill(z);

	int bcode = (*vtx)->barcode();
	m_vtx_x->push_back(x);
	m_vtx_y->push_back(y);
	m_vtx_r->push_back(r);
	m_vtx_z->push_back(z);
	m_vtx_barcode->push_back(bcode);

	if ((*vtx)->barcode() > -20000) {
	  m_h_vtx_prim_xy->Fill(x,y);
	  m_h_vtx_prim_zr->Fill(z,r);
	  ++nvtx;
	}
	else {
	  m_h_vtx_sec_xy->Fill(x,y);
	  m_h_vtx_sec_zr->Fill(z,r);
	  ++nvtx_sec; 
	}
      } //End iteration over vertices

      m_h_n_vert->Fill(nvtx+nvtx_sec);
      m_h_n_vert_prim->Fill(nvtx);
      m_h_n_vert_sec->Fill(nvtx_sec);

      int npart_prim=0; 
      int npart_sec=0;
      HepMC::GenEvent::particle_const_iterator currentGenParticleIter;
      for (currentGenParticleIter=(*currentGenEventIter)->particles_begin(); currentGenParticleIter!=(*currentGenEventIter)->particles_end(); ++currentGenParticleIter) {
	const HepMC::FourVector mom = (*currentGenParticleIter)->momentum();

	m_h_truth_px->Fill(mom.x());
	m_h_truth_py->Fill(mom.y());
	m_h_truth_pz->Fill(mom.z());
	m_h_truth_pt->Fill(mom.perp());
	m_h_truth_eta->Fill(mom.eta());
	m_h_truth_phi->Fill(mom.phi());
	m_h_barcode->Fill((*currentGenParticleIter)->barcode());
	m_h_part_status->Fill((*currentGenParticleIter)->status());
	m_truth_px->push_back(mom.x());
	m_truth_py->push_back(mom.y());
	m_truth_pz->push_back(mom.z());
	m_truth_pt->push_back(mom.perp());
	m_truth_eta->push_back(mom.eta());
	m_truth_phi->push_back(mom.phi());
	m_barcode->push_back((*currentGenParticleIter)->barcode());		
	m_status->push_back((*currentGenParticleIter)->status());
	
	int pdg = (*currentGenParticleIter)->pdg_id();
	m_pdgid->push_back(pdg);
	
	if ((*currentGenParticleIter)->barcode() < 200000) {
	  m_h_part_pdgid->Fill(pdg);
	  m_h_part_p->Fill(mom.rho());
	  m_h_part_eta->Fill(mom.eta());
	  m_h_part_phi->Fill(mom.phi());
	  ++npart_prim; 
	  if ((*currentGenParticleIter)->barcode() < 10000) {
	    m_h_n_generations->Fill(0);
	  }
	  else {
	    m_h_n_generations->Fill(1);
	  }
	} //End barcode <200000
	else {
	  m_h_part_pdgid_sec->Fill(pdg);
	  ++npart_sec;
	  const int gen = (*currentGenParticleIter)->barcode()/1000000 + 2;
	  m_h_n_generations->Fill(gen);    
	}
      } // End iteration over particles

      m_h_n_part_prim->Fill(npart_prim);
      m_h_n_part_sec->Fill(npart_sec);
      m_h_n_part->Fill(npart_prim+npart_sec);	
    } // End mcCollection 
  } // End statuscode success upon retrieval of events
 
  if (m_tree) m_tree->Fill();

  return StatusCode::SUCCESS;
}
