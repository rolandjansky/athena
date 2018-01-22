/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDCHitAnalysis.h"

#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h"
#include "ZDC_SimEvent/ZDC_SimStripHit.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit_Collection.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit.h"

#include "TH1.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <stdio.h>

ZDCHitAnalysis::ZDCHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_h_zdc_sidea_0(0)
  , m_h_zdc_sidea_1(0)
  , m_h_zdc_sidea_2(0)
  , m_h_zdc_sidea_3(0)
  , m_h_zdc_sidec_0(0)
  , m_h_zdc_sidec_1(0)
  , m_h_zdc_sidec_2(0)
  , m_h_zdc_sidec_3(0)
  , m_zdc_strip_side(0)
  , m_zdc_strip_mod(0)
  , m_zdc_strip_energy(0)
  , m_zdc_pix_side(0)
  , m_zdc_pix_mod(0)
  , m_zdc_pix_energy(0)
  , m_tree(0)
  , m_ntupleFileName("/ZDCHitAnalysis/")
  , m_path("/ZDCHitAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path);
}

StatusCode ZDCHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing ZDCHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());
 
  /** Histograms*/
  m_h_zdc_sidea_0 = new TH1D("m_edep_side_a0","edep_side_a0", 100,0, 1000);
  m_h_zdc_sidea_0->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidea_0->GetName(), m_h_zdc_sidea_0));

  m_h_zdc_sidea_1 = new TH1D("m_edep_side_a1","edep_side_a1", 100,0, 1000);
  m_h_zdc_sidea_1->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidea_1->GetName(), m_h_zdc_sidea_1));

  m_h_zdc_sidea_2 = new TH1D("m_edep_side_a2","edep_side_a2", 100,0, 1000);
  m_h_zdc_sidea_2->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidea_2->GetName(), m_h_zdc_sidea_2));

  m_h_zdc_sidea_3 = new TH1D("m_edep_side_a3","edep_side_a3", 100,0, 1000);
  m_h_zdc_sidea_3->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidea_3->GetName(), m_h_zdc_sidea_3));

  m_h_zdc_sidec_0 = new TH1D("m_edep_side_c0","edep_side_c0", 100,0, 1000);
  m_h_zdc_sidec_0->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidec_0->GetName(), m_h_zdc_sidec_0));

  m_h_zdc_sidec_1 = new TH1D("m_edep_side_c1","edep_side_c1", 100,0, 1000);
  m_h_zdc_sidec_1->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidec_1->GetName(), m_h_zdc_sidec_1));

  m_h_zdc_sidec_2 = new TH1D("m_edep_side_c2","edep_side_c2", 100,0, 1000);
  m_h_zdc_sidec_2->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidec_2->GetName(), m_h_zdc_sidec_2));

  m_h_zdc_sidec_3 = new TH1D("m_edep_side_c3","edep_side_c3", 100,0, 1000);
  m_h_zdc_sidec_3->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_zdc_sidec_3->GetName(), m_h_zdc_sidec_3));


  /** now add branches and leaves to the tree */
  m_tree = new TTree("ZDC","ZDC");
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/";
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));
  
  if (m_tree) {
    m_tree->Branch("strip_side", &m_zdc_strip_side);
    m_tree->Branch("strip_mode", &m_zdc_strip_mod);
    m_tree->Branch("strip_energy", &m_zdc_strip_energy);

    m_tree->Branch("pix_side", &m_zdc_pix_side);
    m_tree->Branch("pix_mode", &m_zdc_pix_mod);
    m_tree->Branch("pix_energy", &m_zdc_pix_energy);  
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  return StatusCode::SUCCESS;
}


StatusCode ZDCHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In ZDCHitAnalysis::execute()" );
  
  m_zdc_strip_side->clear();
  m_zdc_strip_mod->clear();
  m_zdc_strip_energy->clear();
  m_zdc_pix_side->clear();
  m_zdc_pix_mod->clear();
  m_zdc_pix_energy->clear();

  double ene_strip = -999.;
  int side_strip = -1;
  int mod_strip = -1;
  double ene_pix = -999.;
  int side_pix = -1;
  int mod_pix = -1;

  ZDC_SimStripHit_ConstIterator striphi;
  const DataHandle<ZDC_SimStripHit_Collection> stripiter;
  CHECK(evtStore()->retrieve(stripiter,"ZDC_SimStripHit_Collection"));
  for (striphi=(*stripiter).begin(); striphi != (*stripiter).end(); ++striphi) {
    ZDC_SimStripHit ghit(*striphi);
    ene_strip = ghit.GetEdep();
    side_strip = ghit.GetSide();
    mod_strip = ghit.GetMod();
    
    if (side_pix==1) {
      switch (mod_strip) {
      case 0:
	m_h_zdc_sidea_0->Fill(ene_strip);
	break;
      case 1:
	m_h_zdc_sidea_1->Fill(ene_strip);
	break;
      case 2:
	m_h_zdc_sidea_2->Fill(ene_strip);
	break;
      case 3:
	m_h_zdc_sidea_3->Fill(ene_strip);
	break;
      }
    }
    else {
      switch (mod_strip) {
      case 0:
	m_h_zdc_sidec_0->Fill(ene_strip);
	break;
      case 1:
	m_h_zdc_sidec_1->Fill(ene_strip);
	break;
      case 2:
	m_h_zdc_sidec_2->Fill(ene_strip);
	break;
      case 3:
	m_h_zdc_sidec_3->Fill(ene_strip);
	break;
      }
    }
  }

  ZDC_SimPixelHit_ConstIterator pixelhi;
  const DataHandle<ZDC_SimPixelHit_Collection> pixeliter;
  CHECK(evtStore()->retrieve(pixeliter,"ZDC_SimPixelHit_Collection"));
  for (pixelhi=(*pixeliter).begin(); pixelhi != (*pixeliter).end(); ++pixelhi) {
    ZDC_SimPixelHit ghit(*pixelhi);
    ene_pix = ghit.GetEdep();
    side_pix = ghit.GetSide();
    mod_pix = ghit.GetMod();
    
    if (side_pix==1) {
      switch (mod_pix) {
      case 0:
	m_h_zdc_sidea_0->Fill(ene_pix);
	break;
      case 1:
	m_h_zdc_sidea_1->Fill(ene_pix);
	break;
      case 2:
	m_h_zdc_sidea_2->Fill(ene_pix);
	break;
      case 3:
	m_h_zdc_sidea_3->Fill(ene_pix);
	break;
      }
    }
    else {
      switch (mod_pix) {
      case 0:
	m_h_zdc_sidec_0->Fill(ene_pix);
	break;
      case 1:
	m_h_zdc_sidec_1->Fill(ene_pix);
	break;
      case 2:
	m_h_zdc_sidec_2->Fill(ene_pix);
	break;
      case 3:
	m_h_zdc_sidec_3->Fill(ene_pix);
	break;
      }
    }
  
    m_zdc_strip_side->push_back(side_strip);
    m_zdc_strip_mod->push_back(mod_strip);
    m_zdc_strip_energy->push_back(ene_strip);
    m_zdc_pix_side->push_back(side_pix);
    m_zdc_pix_mod->push_back(mod_pix);
    m_zdc_pix_energy->push_back(ene_pix);
  }

  if (m_tree) m_tree->Fill();
  
  return StatusCode::SUCCESS;
}
