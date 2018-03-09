/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCHitAnalysis.h"


/*Includes specific to sTGCs*/
#include "GeoAdaptors/GeoMuonHits.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
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

sTGCHitAnalysis::sTGCHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_h_S_rz_A(0)
  , m_h_S_rz_C(0)
  , m_h_S_xy_A(0)
  , m_h_S_xy_C(0)
  , m_h_L_rz_A(0)
  , m_h_L_rz_C(0)
  , m_h_L_xy_A(0)
  , m_h_L_xy_C(0)
  , m_h_rz_A(0)
  , m_h_rz_C(0)
  , m_h_xy_A(0)
  , m_h_xy_C(0)
  , m_h_r_A(0)
  , m_h_r_C(0)
  , m_ntupleFileName("sTGCHitAnalysis/ntuple/")
  , m_path("/sTGCHitAnalysis/histos/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path);

}

StatusCode sTGCHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing sTGCHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());


  /** Histograms */

  m_h_rz_A = new TH2D("h_stgc_rz_A","rz_A", 2000,6500., 8500., 1000, -5000., 5000.);
  m_h_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_rz_A->GetName(), m_h_rz_A));

  m_h_rz_C = new TH2D("h_stgc_rz_C","rz_C", 2000,-8500., -6500., 5000, 0., 5000.);
  m_h_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_rz_C->GetName(), m_h_rz_C));

  m_h_L_rz_A = new TH2D("h_stgc_L_rz_A","L_rz_A", 2000, 6500., 8500., 5000, 0., 5000.);
  m_h_L_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_rz_A->GetName(), m_h_L_rz_A));

  m_h_L_rz_C = new TH2D("h_stgc_L_rz_C","L_rz_C", 2000,-8500., -6500., 5000, 0., 5000.);
  m_h_L_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_rz_C->GetName(), m_h_L_rz_C));

  m_h_S_rz_A = new TH2D("h_stgc_S_rz_A","S_rz_A", 2000,6500., 8500., 5000, 0., 5000.);
  m_h_S_rz_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_rz_A->GetName(), m_h_S_rz_A));

  m_h_S_rz_C = new TH2D("h_stgc_S_rz_C","S_rz_C", 2000, -8500., -6500., 5000, 0., 5000.);
  m_h_S_rz_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_rz_C->GetName(), m_h_S_rz_C));

  
  m_h_xy_A = new TH2D("h_stgc_xy_A","xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_xy_A->GetName(), m_h_xy_A));

  m_h_xy_C = new TH2D("h_stgc_xy_C","xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_xy_C->GetName(), m_h_xy_C));

  m_h_L_xy_A = new TH2D("h_stgc_L_xy_A","L_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_xy_A->GetName(), m_h_L_xy_A));

  m_h_L_xy_C = new TH2D("h_stgc_L_xy_C","L_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_L_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_L_xy_C->GetName(), m_h_L_xy_C));

  m_h_S_xy_A = new TH2D("h_stgc_S_xy_A","S_xy_A", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S_xy_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_xy_A->GetName(), m_h_S_xy_A));

  m_h_S_xy_C = new TH2D("h_stgc_S_xy_C","S_xy_C", 1200,-6000., 6000., 1200, -6000., 6000.);
  m_h_S_xy_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_S_xy_C->GetName(), m_h_S_xy_C));

  m_h_r_A = new TH1D("h_stgc_r_A","r_A", 1000,0., 14000.);
  m_h_r_A->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_r_A->GetName(), m_h_r_A));
  
  m_h_r_C = new TH1D("h_stgc_r_C","r_C", 1000,0., 14000.);
  m_h_r_C->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + m_h_r_C->GetName(), m_h_r_C));

  
  return StatusCode::SUCCESS;
}

StatusCode sTGCHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In sTGCHitAnalysis::execute()" );

  const DataHandle<GenericMuonSimHitCollection> p_collection;;
  if ((evtStore()->retrieve(p_collection,"sTGCSensitiveDetector"))==StatusCode::SUCCESS) {
    for (GenericMuonSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit){

      Amg::Vector3D p = (*i_hit).globalPosition();
      //Get station names and make plots for each wedge
      sTgcHitIdHelper* hitHelper = sTgcHitIdHelper::GetHelper();
      int simId = (*i_hit).GenericId();
      std::string sim_stationName = hitHelper->GetStationName(simId);

      static std::string TS11("TS11");
      static std::string TS21("TS21");
      static std::string TS31("TS31");
      static std::string TL11("TL11");
      static std::string TL21("TL21");
      static std::string TL31("TL31");

      int sim_side = hitHelper->GetSide(simId);

      if ( sim_side==1){ //Both sectors A side
        double r_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r 
        m_h_rz_A->Fill(p.z(), r_A);
	m_h_xy_A->Fill(p.x(), p.y());
	m_h_r_A->Fill(r_A);
      }

      if ( sim_side==-1){ //Both sectors C side
        double r_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_rz_C->Fill(p.z(), r_C);
	m_h_xy_C->Fill(p.x(), p.y());
	m_h_r_C->Fill(r_C);
      }

      if ((sim_stationName==TS11 || sim_stationName==TS21 || sim_stationName==TS31) && sim_side==1){ //Small sectors A side 
        double r_S_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_S_rz_A->Fill(p.z(), r_S_A);
	m_h_S_xy_A->Fill(p.x(), p.y());
      }

      if ((sim_stationName==TS11 || sim_stationName==TS21 || sim_stationName==TS31) && sim_side==-1){ //Small sectors C side 
        double r_S_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_S_rz_C->Fill(p.z(), r_S_C);
        m_h_S_xy_C->Fill(p.x(), p.y());
      }

      if ((sim_stationName==TL11 || sim_stationName==TL21 || sim_stationName==TL31) && sim_side==1){ //Large sectors A side
	double r_L_A = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_L_rz_A->Fill(p.z(), r_L_A);
        m_h_L_xy_A->Fill(p.x(), p.y());
      }

      if ((sim_stationName==TL11 || sim_stationName==TL21 || sim_stationName==TL31) && sim_side==-1){ //Large sectors C side
        double r_L_C = sqrt(p.x()*p.x()+p.y()*p.y()); //Evaluate r
        m_h_L_rz_C->Fill(p.z(), r_L_C);
        m_h_L_xy_C->Fill(p.x(), p.y());
      }

    }
  }
  return StatusCode::SUCCESS;
}
