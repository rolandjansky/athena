/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LucidHitAnalysis.h"

#include "LUCID_SimEvent/LUCID_SimHitCollection.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <stdio.h>

LucidHitAnalysis::LucidHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_h_hit_x(0)
   , m_h_hit_y(0)
   , m_h_hit_z(0)
   , m_h_xy(0)
   , m_h_zr(0)
   , m_h_hit_post_x(0)
   , m_h_hit_post_y(0)
   , m_h_hit_post_z(0)
   , m_h_hit_edep(0)
   , m_h_hit_pdgid(0)
   , m_h_hit_pretime(0)
   , m_h_hit_posttime(0)
   , m_h_genvolume(0)
   , m_h_wavelength(0)
   , m_hit_x(0)
   , m_hit_y(0)
   , m_hit_z(0)
   , m_hit_post_x(0)
   , m_hit_post_y(0)
   , m_hit_post_z(0)
   , m_hit_edep(0)
   , m_hit_pdgid(0)
   , m_hit_pretime(0)
   , m_hit_posttime(0)
   , m_gen_volume(0)
   , m_wavelength(0)
     
   , m_tree(0)
   , m_ntupleFileName("/LucidHitAnalysis/")
   , m_path("/LucidHitAnalysis/")
   , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path); 
}


StatusCode LucidHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing LucidHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  /** Histograms**/
  m_h_hit_x = new TH1D("h_hit_x", "hit_x", 100,-150.,150.);
  m_h_hit_x->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_x->GetName(), m_h_hit_x));

  m_h_hit_y = new TH1D("h_hit_y", "hit_y", 100,-150.,150.);
  m_h_hit_y->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_y->GetName(), m_h_hit_y));

  m_h_hit_z = new TH1D("h_hit_z", "hit_z", 100,-20000.,20000.);
  m_h_hit_z->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_z->GetName(), m_h_hit_z));

  m_h_xy = new TH2D("h_xy", "hit_xy", 100,-150.,150.,100,-150,150);
  m_h_xy->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_xy->GetName(), m_h_xy));

  m_h_zr = new TH2D("h_zr", "hit_zr", 100,-20000.,20000.,100,0,250);
  m_h_zr->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_zr->GetName(), m_h_zr));

  m_h_hit_post_x = new TH1D("h_hit_post_x", "hit_post_x", 100,-150.,150.);
  m_h_hit_post_x->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_post_x->GetName(), m_h_hit_post_x));

  m_h_hit_post_y = new TH1D("h_hit_post_y", "hit_post_y", 100,-150,150.);
  m_h_hit_post_y->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_post_y->GetName(), m_h_hit_post_y));

  m_h_hit_post_z = new TH1D("h_hit_post_z", "hit_post_z", 100,-15000,15000.);
  m_h_hit_post_z->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_post_z->GetName(), m_h_hit_post_z));

  m_h_hit_edep = new TH1D("h_hit_edep", "hit_edep", 100,0.,20.);
  m_h_hit_edep->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_edep->GetName(), m_h_hit_edep));

  m_h_hit_pdgid = new TH1D("h_hit_pdgid", "hit_pdgid", 100,0.,7e6);
  m_h_hit_pdgid->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_pdgid->GetName(), m_h_hit_pdgid));

  m_h_hit_pretime = new TH1D("h_hit_pretime", "hit_pretime", 100,0.,100.);
  m_h_hit_pretime->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_pretime->GetName(), m_h_hit_pretime));

  m_h_hit_posttime = new TH1D("h_hit_posttime", "hit_posttime", 100,0.,100.);
  m_h_hit_posttime->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_hit_posttime->GetName(), m_h_hit_posttime));

  m_h_genvolume = new TH1D("h_genvolume", "genvolume", 20,0.,5.);
  m_h_genvolume->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_genvolume->GetName(), m_h_genvolume));

  m_h_wavelength = new TH1D("m_wavelength", "wavelength", 150,0.,800.);
  m_h_wavelength->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+m_h_wavelength->GetName(), m_h_wavelength));

  /** now add branches and leaves to the tree */
  m_tree = new TTree("Lucid","Lucid");
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/";
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));

  if (m_tree){
    m_tree->Branch("hit_x", &m_hit_x);
    m_tree->Branch("hit_y", &m_hit_y);
    m_tree->Branch("hit_z", &m_hit_z);
    m_tree->Branch("hit_post_x", &m_hit_post_x);
    m_tree->Branch("hit_post_y", &m_hit_post_y);
    m_tree->Branch("hit_post_z", &m_hit_post_z);
    m_tree->Branch("edep", &m_hit_edep);
    m_tree->Branch("pdgid", &m_hit_pdgid);
    m_tree->Branch("pretime", &m_hit_pretime);
    m_tree->Branch("posttime", &m_hit_posttime);
    m_tree->Branch("gen_volume", &m_gen_volume);
    m_tree->Branch("wavelength", &m_wavelength);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }
  
  return StatusCode::SUCCESS;
}		 
  

StatusCode LucidHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In LucidHitAnalysis::execute()" );

  m_hit_x->clear();
  m_hit_y->clear();
  m_hit_z->clear();
  m_hit_post_x->clear();
  m_hit_post_y->clear();
  m_hit_post_z->clear();
  m_hit_edep->clear();
  m_hit_pdgid->clear();
  m_hit_pretime->clear();
  m_hit_posttime->clear();
  m_gen_volume->clear();
  m_wavelength->clear();

  const DataHandle<LUCID_SimHitCollection> iter;
  if (evtStore()->retrieve(iter) == StatusCode::SUCCESS) {
    for (LUCID_SimHitCollection::const_iterator i_hit = (*iter).begin(); i_hit != (*iter).end(); ++i_hit) {
      double x = i_hit->GetX();
      double y = i_hit->GetY();
      double z = i_hit->GetZ();
      double r = sqrt(x*x+y*y);
      
      m_h_hit_x->Fill(i_hit->GetX());
      m_h_hit_y->Fill(i_hit->GetY());
      m_h_hit_z->Fill(i_hit->GetZ());
      m_h_xy->Fill(x, y);
      m_h_zr->Fill(z, r);
      m_h_hit_post_x->Fill(i_hit->GetEPX());
      m_h_hit_post_y->Fill(i_hit->GetEPY());
      m_h_hit_post_z->Fill(i_hit->GetEPZ());
      m_h_hit_edep->Fill(i_hit->GetEnergy());
      m_h_hit_pdgid->Fill(i_hit->GetPdgCode());
      m_h_hit_pretime->Fill(i_hit->GetPreStepTime());
      m_h_hit_posttime->Fill(i_hit->GetPostStepTime());
      m_h_genvolume->Fill(i_hit->GetGenVolume());
      m_h_wavelength->Fill(i_hit->GetWavelength());

      m_hit_x->push_back(i_hit->GetX());
      m_hit_y->push_back(i_hit->GetY());
      m_hit_z->push_back(i_hit->GetZ());
      m_hit_post_x->push_back(i_hit->GetEPX());
      m_hit_post_y->push_back(i_hit->GetEPY());
      m_hit_post_z->push_back(i_hit->GetEPZ());
      m_hit_edep->push_back(i_hit->GetEnergy());
      m_hit_pdgid->push_back(i_hit->GetPdgCode());
      m_hit_pretime->push_back(i_hit->GetPreStepTime());
      m_hit_posttime->push_back(i_hit->GetPostStepTime());
      m_gen_volume->push_back(i_hit->GetGenVolume());
      m_wavelength->push_back(i_hit->GetWavelength()); 
    }
  } // End while hits
   
  if (m_tree) m_tree->Fill();

  return StatusCode::SUCCESS;
  
}
