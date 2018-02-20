/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SiHitAnalysis.h"

// Section of includes for Pixel and SCT tests
#include "InDetSimEvent/SiHitCollection.h"
#include "GeoAdaptors/GeoSiHit.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

//m_collection:  "PixelHits", "SCT_Hits", "BCMHits" and "BLMHits" 
SiHitAnalysis::SiHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_h_hits_x(0)
   , m_h_hits_y(0)
   , m_h_hits_z(0)
   , m_h_hits_r(0)
   , m_h_xy(0)
   , m_h_zr(0)
   , m_h_hits_time(0)
   , m_h_hits_eloss(0)
   , m_h_hits_step(0)
   , m_h_hits_barcode(0)
   , m_h_time_eloss(0)
   , m_h_z_eloss(0)
   , m_h_r_eloss(0)
   , m_hits_x(0)
   , m_hits_y(0)
   , m_hits_z(0)
   , m_hits_r(0)
   , m_hits_time(0)
   , m_hits_eloss(0)
   , m_hits_step(0)
   , m_hits_barcode(0)
     
   , m_collection("BCMHits")
   , m_expert("off")
     
   , m_tree(0)
   , m_ntupleFileName("/SiHitAnalysis/")
   , m_path("/SiHitAnalysis/")
   , m_thistSvc("THistSvc", name)
{
  declareProperty("CollectionName",  m_collection="BCMHits");
  declareProperty("ExpertMode", m_expert = "off");
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path);
}

StatusCode SiHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing SiHitAnalysis" );

  std::string detName("Pixel");
  std::string ntupName("SiPixel");
  
  if (m_collection=="PixelHits") {
    detName = "Pixel";
    ntupName = "SiPixel";
  }
  else if (m_collection=="SCT_Hits") {
    detName = "SCT";
    ntupName = "SiSCT";
  }
  else if (m_collection=="BCMHits") {
    detName = "BCM";
    ntupName = "SiBCM";
  }
  else if (m_collection=="BLMHits") {
    detName = "BLM";
    ntupName = "SiBLM";
  }
  else {
    ATH_MSG_ERROR("SiHitsAnalysis for "<< name()<<"not supported!!! \n");
    return StatusCode::FAILURE;
  }

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  /** Histograms**/
  float bin_down = -600;
  float bin_up = 600;
  float radius_up = 600;
  float radius_down = 200;
  if (detName=="Pixel") {
    bin_down = -170;
    bin_up = 170;
    radius_up = 170;
    radius_down = 0;
  }
  
  m_h_hits_x = new TH1D(("h_"+detName+"_x").c_str(),("h_"+detName+"_x").c_str(), 100,bin_down, bin_up);
  m_h_hits_x->StatOverflows();

  m_h_hits_y = new TH1D(("h_"+detName+"_y").c_str(), ("h_"+detName+"_y").c_str(), 100,bin_down,bin_up);
  m_h_hits_y->StatOverflows();

  m_h_hits_z = new TH1D(("h_"+detName+"_z").c_str(), ("h_"+detName+"_z").c_str(), 200,-1200,1200);
  m_h_hits_z->StatOverflows();

  m_h_hits_r = new TH1D(("h_"+detName+"_r").c_str(), ("h_"+detName+"_r").c_str(), 100,radius_down,radius_up);
  m_h_hits_r->StatOverflows();

  m_h_xy = new TH2D(("h_"+detName+"_xy").c_str(), ("h_"+detName+"_xy").c_str(), 100,bin_down,bin_up,100, bin_down, bin_up);
  m_h_xy->StatOverflows();

  m_h_zr = new TH2D(("h_"+detName+"_zr").c_str(), ("h_"+detName+"_zr").c_str(), 100,-1200,1200.,100, radius_down, radius_up);
  m_h_zr->StatOverflows();

  m_h_hits_time = new TH1D(("h_"+detName+"_time").c_str(), ("h_"+detName+"_time").c_str(), 100,0,500);
  m_h_hits_time->StatOverflows();

  m_h_hits_eloss = new TH1D(("h_"+detName+"_eloss").c_str(), ("h_"+detName+"_eloss").c_str(), 100,0,50);
  m_h_hits_eloss->StatOverflows();

  m_h_hits_step = new TH1D(("h_"+detName+"_step").c_str(), ("h_"+detName+"_step").c_str(), 100,0,50);
  m_h_hits_step->StatOverflows();

  m_h_hits_barcode = new TH1D(("h_"+detName+"_barcode").c_str(), ("h_"+detName+"_barcode").c_str(), 200,0,250000);
  m_h_hits_barcode->StatOverflows();

  m_h_time_eloss = new TH2D(("h_"+detName+"_time_eloss").c_str(), ("h_"+detName+" Eloss vs. time").c_str(),100, 0,500,100,0,50);
  m_h_time_eloss->StatOverflows();

  m_h_z_eloss = new TH2D(("h_"+detName+"_z_eloss").c_str(), ("h_"+detName+" Eloss vs. z").c_str(),100, -1200,1200,100,0,50);
  m_h_z_eloss->StatOverflows();

  m_h_r_eloss = new TH2D(("h_"+detName+"_r_eloss").c_str(), ("h_"+detName+ " Eloss vs. r").c_str(),100, radius_down,radius_down,100,0,50);
  m_h_r_eloss->StatOverflows();

  CHECK(m_thistSvc->regHist(m_path + m_h_hits_x->GetName(), m_h_hits_x));
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_y->GetName(), m_h_hits_y));
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_z->GetName(), m_h_hits_z));
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_r->GetName(), m_h_hits_r));
  CHECK(m_thistSvc->regHist( m_path+m_h_xy->GetName(), m_h_xy));
  CHECK(m_thistSvc->regHist( m_path+m_h_zr->GetName(), m_h_zr));
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_time->GetName(), m_h_hits_time));
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_eloss->GetName(), m_h_hits_eloss));
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_step->GetName(), m_h_hits_step));
  CHECK(m_thistSvc->regHist(m_path + m_h_hits_barcode->GetName(), m_h_hits_barcode));
  
  //To be filled only when the expert mode is on.
  if (m_expert == "on") {
    CHECK(m_thistSvc->regHist(m_path + m_h_time_eloss->GetName(), m_h_time_eloss));
    CHECK(m_thistSvc->regHist(m_path + m_h_z_eloss->GetName(), m_h_z_eloss));
    CHECK(m_thistSvc->regHist(m_path + m_h_r_eloss->GetName(), m_h_r_eloss));
  }

  CHECK(m_thistSvc.retrieve());

  /** now add branches and leaves to the tree */
  m_tree = new TTree(ntupName.c_str(), ntupName.c_str());
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/" + detName;
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));

  if (m_tree){
    m_tree->Branch((detName+"_x").c_str(), &m_hits_x);
    m_tree->Branch((detName+"_y").c_str(), &m_hits_y);
    m_tree->Branch((detName+"_z").c_str(), &m_hits_z);
    m_tree->Branch((detName+"_r").c_str(), &m_hits_r);
    m_tree->Branch((detName+"_time").c_str(), &m_hits_time);
    m_tree->Branch((detName+"_eloss").c_str(), &m_hits_eloss);
    m_tree->Branch((detName+"_step").c_str(), &m_hits_step);
    m_tree->Branch((detName+"_barcode").c_str(), &m_hits_barcode);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }
  
  return StatusCode::SUCCESS;
}		 


StatusCode SiHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In SiHitAnalysis::execute()" );
  
  m_hits_x->clear();
  m_hits_y->clear();
  m_hits_z->clear();
  m_hits_r->clear();
  m_hits_time->clear();
  m_hits_eloss->clear();
  m_hits_step->clear();
  m_hits_barcode->clear();
  
  const DataHandle<SiHitCollection> p_collection;
  if (evtStore()->retrieve(p_collection, m_collection) == StatusCode::SUCCESS) {
    for (SiHitConstIterator i_hit = p_collection->begin(); i_hit != p_collection->end() ;++i_hit) {
      GeoSiHit ghit(*i_hit);
      HepGeom::Point3D<double> p = ghit.getGlobalPosition();
      m_h_hits_x->Fill(p.x());
      m_h_hits_y->Fill(p.y());
      m_h_hits_z->Fill(p.z());
      m_h_hits_r->Fill(p.perp());
      m_h_xy->Fill(p.x(), p.y());
      m_h_zr->Fill(p.z(),p.perp());
      m_h_hits_eloss->Fill(i_hit->energyLoss());
      m_h_hits_time->Fill(i_hit->meanTime());  
      double step_length=(i_hit->localStartPosition() - i_hit->localEndPosition()).mag();
      m_h_hits_step->Fill(step_length);
      m_h_hits_barcode->Fill(i_hit->particleLink().barcode());
      
      if (m_expert == "on") {
	m_h_time_eloss->Fill(i_hit->meanTime(), i_hit->energyLoss());
	if (i_hit->getBarrelEndcap()==0) {
	  m_h_z_eloss->Fill(p.z(), i_hit->energyLoss());
	}
	else {
	  m_h_r_eloss->Fill(p.perp(), i_hit->energyLoss());
	}
      }
      
      m_hits_x->push_back(p.x());
      m_hits_y->push_back(p.y());
      m_hits_z->push_back(p.z());
      m_hits_r->push_back(p.perp());
      m_hits_eloss->push_back(i_hit->energyLoss());
      m_hits_time->push_back(i_hit->meanTime()); 
      m_hits_step->push_back(step_length);
      m_hits_barcode->push_back(i_hit->particleLink().barcode());    
    } // End while hits
  } // End statuscode success upon retrieval of hits

  if (m_tree) m_tree->Fill();

  return StatusCode::SUCCESS;
}
