/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFAHitAnalysis.h"

// Section of includes for Pixel and SCT tests
#include "ALFA_SimEv/ALFA_HitCollection.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

ALFAHitAnalysis::ALFAHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_station(0)
   , m_plate(0)
   , m_fiber(0)
   , m_sign(0)
   , m_energy(0)
     
   , m_tree(0)
   , m_ntupleFileName("/ALFAHitsAnalysis/")
   , m_path("/ALFAHitsAnalysis/")
   , m_thistSvc("THistSvc", name)
{ 
  for (int i(0); i<8; i++) {
    m_h_E_full_sum_h[i]=0;
    m_h_E_layer_sum_h[i]=0;
    m_h_hit_layer[i]=0;
    m_h_hit_fiber[i]=0;
  }
  declareProperty("NtupleFileName", m_ntupleFileName);    
  declareProperty("HistPath", m_path); 
}


StatusCode ALFAHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing ALFAHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());
 
  /** now add branches and leaves to the tree */
  std::stringstream s;
  for (unsigned int j=0; j<8; j++) {
    s.str("");
    s << "edep_in_det_no." << j+1;
    float Emax = 5;
    if (j==3) Emax = 150;
    m_h_E_full_sum_h[j] = new TH1D(s.str().c_str(), s.str().c_str(), 100, 0, Emax);
    m_h_E_full_sum_h[j]->StatOverflows();
    CHECK( m_thistSvc->regHist( m_path + m_h_E_full_sum_h[j]->GetName(), m_h_E_full_sum_h[j] ) );

    s.str("");
    s << "edep_per_layer_det_no." << j+1;
    m_h_E_layer_sum_h[j] = new TH1D(s.str().c_str(), s.str().c_str(), 100, 0, 15);
    m_h_E_layer_sum_h[j]->StatOverflows();
    CHECK( m_thistSvc->regHist( m_path + m_h_E_layer_sum_h[j]->GetName(), m_h_E_layer_sum_h[j] ) );

    s.str("");
    s << "hit_layer_det_no." << j+1;
    m_h_hit_layer[j] = new TH1D(s.str().c_str(), s.str().c_str(), 50, 0, 50);
    m_h_hit_layer[j]->StatOverflows();
    CHECK( m_thistSvc->regHist( m_path + m_h_hit_layer[j]->GetName(), m_h_hit_layer[j] ) );

    s.str("");
    s << "hit_fiber_det_no." << j+1;
    m_h_hit_fiber[j] = new TH1D(s.str().c_str(), s.str().c_str(), 100, 0, 60);
    m_h_hit_fiber[j]->StatOverflows();
    CHECK( m_thistSvc->regHist( m_path + m_h_hit_fiber[j]->GetName(), m_h_hit_fiber[j] ) );
  }

  m_tree = new TTree("ALFA", "ALFA");
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/" ;
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));
  
  if (m_tree) {
      m_tree->Branch("station", &m_station);
      m_tree->Branch("plate", &m_plate);
      m_tree->Branch("fiber", &m_fiber);
      m_tree->Branch("energy", &m_energy);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }
  
  return StatusCode::SUCCESS;
}		 


StatusCode ALFAHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In ALFAHitAnalysis::execute()" );
  
  m_station->clear();
  m_plate->clear();
  m_fiber->clear();
  m_sign->clear();
  m_energy->clear();

  //cleaning
  int fiber, plate, sign, station;
  double E_fiber_sum[8][10][64][2], E_full_sum[8], E_layer_sum[8][20];
  for (int l= 0; l<8; l++) {
    E_full_sum[l] = 0.;           
    for (int i = 0; i < 10; i++) {
      E_layer_sum[l][i] = 0.;
      E_layer_sum[l][i+10] = 0.;
      for (int j = 0; j < 64; j++) {
	for (int k = 0; k < 2; k++) {
	  E_fiber_sum[l][i][j][k] = 0.;
	}
      }
    }
  }
  
  ALFA_HitConstIter iter;
  const DataHandle<ALFA_HitCollection> col_alfa;
  CHECK( evtStore()->retrieve( col_alfa, "ALFA_HitCollection" ) );
  for (iter = (*col_alfa).begin(); iter != (*col_alfa).end(); ++iter) {
    station = (*iter).GetStationNumber();
    plate = (*iter).GetPlateNumber();
    fiber = (*iter).GetFiberNumber();
    sign = (*iter).GetSignFiber();
    E_fiber_sum[station-1][plate-1][fiber-1][(1-sign)/2] += ((*iter).GetEnergyDeposit());

    m_station->push_back(station);
    m_plate->push_back(plate);
    m_fiber->push_back(fiber);
    m_sign->push_back(sign);
    m_energy->push_back((*iter).GetEnergyDeposit());
  } // End iteration
 
  for (int l=0; l<8; l++){
    for (int i=0; i<10; i++){
      for (int j=0; j<64; j++){
	for (int k=0; k<2; k++){
	  E_full_sum[l] += E_fiber_sum[l][i][j][k];
	  E_layer_sum[l][2*i+k] += E_fiber_sum[l][i][j][k];
	  if (E_fiber_sum[l][i][j][k] > 0.) {
	    m_h_hit_layer[l]->Fill(2*i+k+1);
	    m_h_hit_fiber[l]->Fill(j+1);
	  }
	}
      }
    }
  }
  for (int l=0; l<8; l++) {
    m_h_E_full_sum_h[l]->Fill(E_full_sum[l]);
    for (int i = 0; i< 20; i++) {
      m_h_E_layer_sum_h[l]->Fill(E_layer_sum[l][i]);
    }
  }

  if (m_tree) m_tree->Fill();
  
  return StatusCode::SUCCESS;
}
