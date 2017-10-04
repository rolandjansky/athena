/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MMHitAnalysis.h"


/*Includes specific to Micromegas*/
#include "GeoAdaptors/GeoMuonHits.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHit.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <stdio.h>

using namespace std;

MMHitAnalysis::MMHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_h_S1_xy_A(0)
  , m_h_S1_rz_A(0)
  , m_h_S1_r_A(0)
  , m_h_S1_xy_C(0)
  , m_h_S1_rz_C(0)
  , m_h_S1_r_C(0)
  , m_h_S2_xy_A(0)
  , m_h_S2_rz_A(0)
  , m_h_S2_r_A(0)
  , m_h_S2_xy_C(0)
  , m_h_S2_rz_C(0)
  , m_h_S2_r_C(0)
  , m_h_S_xy_A(0)
  , m_h_S_xy_C(0)
  , m_h_S_rz_A(0)
  , m_h_S_rz_C(0)
  , m_h_xy_A(0)
  , m_h_xy_C(0)
  , m_h_rz_A(0)
  , m_h_rz_C(0)
  , m_h_L1_xy_A(0)
  , m_h_L1_rz_A(0)
  , m_h_L1_r_A(0)
  , m_h_L1_xy_C(0)
  , m_h_L1_rz_C(0)
  , m_h_L1_r_C(0)
  , m_h_L2_xy_A(0)
  , m_h_L2_rz_A(0)
  , m_h_L2_r_A(0)
  , m_h_L2_xy_C(0)
  , m_h_L2_rz_C(0)
  , m_h_L2_r_C(0)
  , m_h_L_xy_A(0)
  , m_h_L_xy_C(0)
  , m_h_L_rz_A(0)
  , m_h_L_rz_C(0)  
  , m_hits_x(0)
  , m_ntupleFileName("MMHitAnalysis/ntuple/")
  , m_path("/MMHitAnalysis/histos/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName); 
  declareProperty("HistPath", m_path);

}

StatusCode MMHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing MMHitAnalysis" );

// Grab the Ntuple and histogramming service for the tree                                                                                                                        
  CHECK(m_thistSvc.retrieve());


/** Histograms */

/*-----------------------------------Small sector Wedge 1 histos begin--------------------------------------*/
  m_h_S1_xy_A = new TH2D("h_mm_S1_xy_A","S1_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S1_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S1_xy_A->GetName(), m_h_S1_xy_A));

  m_h_S1_rz_A = new TH2D("h_mm_S1_rz_A","S1_rz_A", 1000,7000., 8000., 5000, 0., 5000.);
  m_h_S1_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S1_rz_A->GetName(), m_h_S1_rz_A));

  m_h_S1_r_A = new TH1D("h_mm_S1_r_A","S1_r_A", 1000,0., 14000.);
  m_h_S1_r_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S1_r_A->GetName(), m_h_S1_r_A));

  m_h_S1_xy_C = new TH2D("h_mm_S1_xy_C","S1_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S1_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S1_xy_C->GetName(), m_h_S1_xy_C));

  m_h_S1_rz_C = new TH2D("h_mm_S1_rz_C","S1_rz_C", 1000,-8000., -7000., 5000, 0., 5000.);
  m_h_S1_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S1_rz_C->GetName(), m_h_S1_rz_C));

  m_h_S1_r_C = new TH1D("h_mm_S1_r_C","S1_r_C", 1000,0., 14000.);
  m_h_S1_r_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S1_r_C->GetName(), m_h_S1_r_C));
  /*-----------------------------------Small sector Wedge 1 histos end--------------------------------------*/

  /*-----------------------------------Small sector Wedge 2 histos begin--------------------------------------*/
  m_h_S2_xy_A = new TH2D("h_mm_S2_xy_A","S2_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S2_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S2_xy_A->GetName(), m_h_S2_xy_A));

  m_h_S2_rz_A = new TH2D("h_mm_S2_rz_A","S2_rz_A", 1000,7000., 8000., 5000, 0., 5000.);
  m_h_S2_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S2_rz_A->GetName(), m_h_S2_rz_A));

  m_h_S2_r_A = new TH1D("h_mm_S2_r_A","S2_r_A", 1000,0., 14000.);
  m_h_S2_r_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S2_r_A->GetName(), m_h_S2_r_A));

  m_h_S2_xy_C = new TH2D("h_mm_S2_xy_C","S2_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S2_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S2_xy_C->GetName(), m_h_S2_xy_C));

  m_h_S2_rz_C = new TH2D("h_mm_S2_rz_C","S2_rz_C", 1000,-8000., -7000., 5000, 0., 5000.);
  m_h_S2_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S2_rz_C->GetName(), m_h_S2_rz_C));

  m_h_S2_r_C = new TH1D("h_mm_S2_r_C","S2_r_C", 1000,0., 14000.);
  m_h_S2_r_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S2_r_C->GetName(), m_h_S2_r_C));
/*---------------------------------Small sector Wedge 2 histos end--------------------------------------*/
  
/*-----------------------------------Small sector both Wedges histos begin--------------------------------------*/
  m_h_S_xy_A = new TH2D("h_mm_S_xy_A","S_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_xy_A->GetName(), m_h_S_xy_A));

  m_h_S_xy_C = new TH2D("h_mm_S_xy_C","S_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_xy_C->GetName(), m_h_S_xy_C));

  m_h_S_rz_A = new TH2D("h_mm_S_rz_A","S_rz_A", 1000,7000., 8000., 5000, 0., 5000.);
  m_h_S_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_rz_A->GetName(), m_h_S_rz_A));

  m_h_S_rz_C = new TH2D("h_mm_S_rz_C","S_rz_C", 1000,-8000., -7000., 5000, 0., 5000.);
  m_h_S_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_rz_C->GetName(), m_h_S_rz_C));
  /*---------------------------------Small sector both Wedges histos end--------------------------------------*/

  /*-----------------------------------All sector histos begin--------------------------------------*/
  m_h_xy_A = new TH2D("h_mm_xy_A","xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_xy_A->GetName(), m_h_xy_A));

  m_h_xy_C = new TH2D("h_mm_xy_C","xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_xy_C->GetName(), m_h_xy_C));

  m_h_rz_A = new TH2D("h_mm_rz_A","rz_A", 1000,7000., 8000., 5000, 0., 5000.);
  m_h_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_rz_A->GetName(), m_h_rz_A));

  m_h_rz_C = new TH2D("h_mm_rz_C","rz_C", 1000,-8000., -7000., 5000, 0., 5000.);
  m_h_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_rz_C->GetName(), m_h_rz_C));
  /*---------------------------------All sector histos end-------------------------------------------*/

  /*-----------------------------------Large sector Wedge 1 histos begin--------------------------------------*/
  m_h_L1_xy_A = new TH2D("h_mm_L1_xy_A","L1_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L1_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L1_xy_A->GetName(), m_h_L1_xy_A));

  m_h_L1_rz_A = new TH2D("h_mm_L1_rz_A","L1_rz_A", 1000,7000., 8000., 5000, 0., 5000.);
  m_h_L1_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L1_rz_A->GetName(), m_h_L1_rz_A));

  m_h_L1_r_A = new TH1D("h_mm_L1_r_A","L1_r_A", 1000,0., 14000.);
  m_h_L1_r_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L1_r_A->GetName(), m_h_L1_r_A));

  m_h_L1_xy_C = new TH2D("h_mm_L1_xy_C","L1_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L1_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L1_xy_C->GetName(), m_h_L1_xy_C));

  m_h_L1_rz_C = new TH2D("h_mm_L1_rz_C","L1_rz_C", 1000,-8000., -7000., 5000, 0., 5000.);
  m_h_L1_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L1_rz_C->GetName(), m_h_L1_rz_C));

  m_h_L1_r_C = new TH1D("h_mm_L1_r_C","L1_r_C", 1000,0., 14000.);
  m_h_L1_r_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L1_r_C->GetName(), m_h_L1_r_C));
  /*-----------------------------------Large sector Wedge 1 histos end--------------------------------------*/

  /*-----------------------------------Large sector Wedge 2 histos begin--------------------------------------*/
  m_h_L2_xy_A = new TH2D("h_mm_L2_xy_A","L2_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L2_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L2_xy_A->GetName(), m_h_L2_xy_A));

  m_h_L2_rz_A = new TH2D("h_mm_L2_rz_A","L2_rz_A", 1000,7000., 8000., 5000, 0., 5000.);
  m_h_L2_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L2_rz_A->GetName(), m_h_L2_rz_A));

  m_h_L2_r_A = new TH1D("h_mm_L2_r_A","L2_r_A", 1000,0., 14000.);
  m_h_L2_r_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L2_r_A->GetName(), m_h_L2_r_A));

  m_h_L2_xy_C = new TH2D("h_mm_L2_xy_C","L2_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L2_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L2_xy_C->GetName(), m_h_L2_xy_C));

  m_h_L2_rz_C = new TH2D("h_mm_L2_rz_C","L2_rz_C", 1000,-8000., -7000., 5000, 0., 5000.);
  m_h_L2_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L2_rz_C->GetName(), m_h_L2_rz_C));

  m_h_L2_r_C = new TH1D("h_mm_L2_r_C","L2_r_C", 1000,0., 14000.);
  m_h_L2_r_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L2_r_C->GetName(), m_h_L2_r_C));
  /*-----------------------------------Large sector Wedge 1 histos end--------------------------------------*/

  /*-----------------------------------Large sector both Wedge histos begin--------------------------------------*/
  m_h_L_xy_A = new TH2D("h_mm_L_xy_A","L_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_xy_A->GetName(), m_h_L_xy_A));

  m_h_L_rz_A = new TH2D("h_mm_L_rz_A","L_rz_A", 1000,7000., 8000., 5000, 0., 5000.);
  m_h_L_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_rz_A->GetName(), m_h_L_rz_A));

  m_h_L_xy_C = new TH2D("h_mm_L_xy_C","L_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_xy_C->GetName(), m_h_L_xy_C));

  m_h_L_rz_C = new TH2D("h_mm_L_rz_C","L_rz_C", 1000,-8000., -7000., 5000, 0., 5000.);
  m_h_L_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_rz_C->GetName(), m_h_L_rz_C));
  /*-----------------------------------Large sector both Wedge histos end--------------------------------------*/

  m_tree= new TTree("NtupleMMHitAnalysis","MMHitAna");
  std::string fullNtupleName =  "/"+m_ntupleFileName+"/";
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));


  /** now add branches and leaves to the tree */

  if (m_tree){
    m_tree->Branch("x", &m_hits_x);
  }
  else{
    ATH_MSG_ERROR("No tree found!");
  }

  return StatusCode::SUCCESS;
}

StatusCode MMHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In MMHitAnalysis::execute()" );

  m_hits_x->clear();

  const DataHandle<GenericMuonSimHitCollection> p_collection;;
  if ((evtStore()->retrieve(p_collection,"MicromegasSensitiveDetector"))==StatusCode::SUCCESS) {
    for (GenericMuonSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit){
      
      Amg::Vector3D p = (*i_hit).globalPosition();
      //Get station names and make plots for each sector
      MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper(); 
      int simId = (*i_hit).GenericId();
      std::string sim_stationName = hitHelper->GetStationName(simId);
      int sim_side = hitHelper->GetSide(simId);
      //Declare station name strings
      static std::string s_m1s1("M1S1");
      static std::string s_m2s1("M2S1");
      static std::string s_m1s2("M1S2");
      static std::string s_m2s2("M2S2");
      static std::string s_m1l1("M1L1");
      static std::string s_m2l1("M2L1");
      static std::string s_m1l2("M1L2");
      static std::string s_m2l2("M2L2");

      //Small sectors
      /*-------------------------Wedge 1 Fill histos begin-------------------------*/
      //M1S1 (Note: M1->Module 1, S1->Small sector wedge 1)

      //Small sectors at +ve Z (A-Side)
      if ((sim_stationName==s_m1s1 || sim_stationName==s_m2s1) && sim_side==1){
	double r_S1_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r 
	m_h_S1_xy_A->Fill(p.x(),p.y());
	m_h_S1_rz_A->Fill(p.z(), r_S1_A);
	m_h_S1_r_A->Fill(r_S1_A);
      }

      //Small sectors at -ve Z (C-Side)                     
      if ((sim_stationName==s_m1s1 || sim_stationName==s_m2s1) && sim_side==-1){
	double r_S1_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r 
        m_h_S1_xy_C->Fill(p.x(),p.y());
        m_h_S1_rz_C->Fill(p.z(), r_S1_C);
        m_h_S1_r_C->Fill(r_S1_C);
      }

      /*------------------------Wedge 1 Fill histos end---------------------------*/

      /*------------------------- Wedge 2 Fill histos begin-------------------------*/
      //Small sectors at +ve Z (A-Side)      
      if ((sim_stationName==s_m1s2 || sim_stationName==s_m2s2) && sim_side==1){
	double r_S2_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_S2_xy_A->Fill(p.x(),p.y());
        m_h_S2_rz_A->Fill(p.z(), r_S2_A);
        m_h_S2_r_A->Fill(r_S2_A);
      }

      //Small sectors at -ve Z (C-Side) 
      if ((sim_stationName==s_m1s2 || sim_stationName==s_m2s2) && sim_side==-1){
        double r_S2_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_S2_xy_C->Fill(p.x(),p.y());
        m_h_S2_rz_C->Fill(p.z(), r_S2_C);
        m_h_S2_r_C->Fill(r_S2_C);
      }

      /*------------------------Wedge 2 Fill histos end---------------------------*/

      /*------------------------- Small sectors both wedges Fill histos begin-------------------------*/
      //Small sectors at +ve Z (A-Side)
      if ((sim_stationName==s_m1s2 || sim_stationName==s_m2s2 || sim_stationName==s_m1s1 || sim_stationName==s_m2s1) && sim_side==1){
        double r_S_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r                                                                                                                
        m_h_S_xy_A->Fill(p.x(),p.y());
        m_h_S_rz_A->Fill(p.z(), r_S_A);
      }

      //Small sectors at -ve Z (C-Side)
      if ((sim_stationName==s_m1s2 || sim_stationName==s_m2s2 || sim_stationName==s_m1s1 || sim_stationName==s_m2s1) && sim_side==-1){
        double r_S_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r                                                                                                                
        m_h_S_xy_C->Fill(p.x(),p.y());
        m_h_S_rz_C->Fill(p.z(), r_S_C);
      }
      /*------------------------Small sectors both wedges Fill histos end---------------------------*/

      //All sectors
      /*-------------------------All sectors Fill histos begin-------------------------*/
      //Sectors at +ve Z (A-Side)
      if (sim_side==1){
        double r_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r                                                                                                                
        m_h_xy_A->Fill(p.x(),p.y());
        m_h_rz_A->Fill(p.z(), r_A);
      }

      //Sectors at -ve Z (C-Side)
      if (sim_side==-1){
        double r_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r                                                                                                                
        m_h_xy_C->Fill(p.x(),p.y());
        m_h_rz_C->Fill(p.z(), r_C);
      }
      /*------------------------All sectors Fill histos end---------------------------*/

      //Large sectors
      /*-------------------------Wedge 1 Fill histos begin-------------------------*/
      //M1L1 (Note: M1->Module 1, L1->Large sector wedge 1)

      //Large sectors at +ve Z (A-Side)
      if ((sim_stationName==s_m1l1 || sim_stationName==s_m2l1) && sim_side==1){
        double r_L1_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_L1_xy_A->Fill(p.x(),p.y());
        m_h_L1_rz_A->Fill(p.z(), r_L1_A);
        m_h_L1_r_A->Fill(r_L1_A);
      }

      //Large sectors at -ve Z (C-Side)
      if ((sim_stationName==s_m1l1 || sim_stationName==s_m2l1) && sim_side==-1){
        double r_L1_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_L1_xy_C->Fill(p.x(),p.y());
        m_h_L1_rz_C->Fill(p.z(), r_L1_C);
        m_h_L1_r_C->Fill(r_L1_C);
      }

      /*------------------------Wedge 1 Fill histos end---------------------------*/

      /*-------------------------Wedge 2 Fill histos begin-------------------------*/
      //M1L1 (Note: M1->Module 1, L1->Large sector wedge 1)

      //Large sectors at +ve Z (A-Side) 
      if ((sim_stationName==s_m1l2 || sim_stationName==s_m2l2) && sim_side==1){
        double r_L2_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_L2_xy_A->Fill(p.x(),p.y());
        m_h_L2_rz_A->Fill(p.z(), r_L2_A);
        m_h_L2_r_A->Fill(r_L2_A);
      }

      //Large sectors at -ve Z (C-Side)
      if ((sim_stationName==s_m1l2 || sim_stationName==s_m2l2) && sim_side==-1){
        double r_L2_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_L2_xy_C->Fill(p.x(),p.y());
        m_h_L2_rz_C->Fill(p.z(), r_L2_C);
        m_h_L2_r_C->Fill(r_L2_C);
      }

      /*------------------------Wedge 2 Fill histos end---------------------------*/

      /*------------------------- Large sectors both wedges Fill histos begin-------------------------*/
      //Large sectors at +ve Z (A-Side)
      if ((sim_stationName==s_m1l2 || sim_stationName==s_m2l2 || sim_stationName==s_m1l1 || sim_stationName==s_m2l1) && sim_side==1){
        double r_L_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r                                                                                                                
        m_h_L_xy_A->Fill(p.x(),p.y());
        m_h_L_rz_A->Fill(p.z(), r_L_A);
      }

      //Large sectors at -ve Z (C-Side)
      if ((sim_stationName==s_m1l2 || sim_stationName==s_m2l2 || sim_stationName==s_m1l1 || sim_stationName==s_m2l1) && sim_side==-1){
        double r_L_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r                                                                                                                
        m_h_L_xy_C->Fill(p.x(),p.y());
        m_h_L_rz_C->Fill(p.z(), r_L_C);
      }
      /*------------------------Large sectors both wedges Fill histos end---------------------------*/

      m_hits_x->push_back(p.x());
    }
  } // End while hits

  if (m_tree) m_tree->Fill();

  return StatusCode::SUCCESS;
}
