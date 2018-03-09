/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFPHitAnalysis.h"

#include "AFP_SimEv/AFP_SIDSimHit.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <stdio.h>

AFPHitAnalysis::AFPHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_h_hitID(0)
   , m_h_pdgID(0)
   , m_h_trackID(0)
   , m_h_kine(0)
   , m_h_edep(0)
   , m_h_stepX(0)
   , m_h_stepY(0)
   , m_h_stepZ(0)
   , m_h_time(0)
   , m_h_stationID(0)
   , m_h_detID(0)
   , m_h_pixelRow(0)
   , m_h_pixelCol(0)
   , m_hitID(0)
   , m_pdgID(0)
   , m_trackID(0)
   , m_kine(0)
   , m_edep(0)
   , m_stepX(0)
   , m_stepY(0)
   , m_stepZ(0)
   , m_time(0)
   , m_stationID(0)
   , m_detID(0)
   , m_pixelRow(0)
   , m_pixelCol(0)
     
   , m_tree(0)
   , m_ntupleFileName("/AFPHitAnalysis/")
   , m_path("/AFPHitAnalysis/")
   , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("HistPath", m_path);
}


StatusCode AFPHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing AFPHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  /** now define the histograms**/
  m_h_hitID = new TH1D("h_hitID", "hitID",100, 0., 100.);
  m_h_hitID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_hitID->GetName(), m_h_hitID));

  m_h_pdgID =  new TH1D("h_pdgID", "pdgID", 200, -100,100);
  m_h_pdgID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_pdgID->GetName(), m_h_pdgID));	

  m_h_trackID =  new TH1D("h_trackID", "trackID", 100, 0,100);
  m_h_trackID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_trackID->GetName(), m_h_trackID));	

  m_h_kine =  new TH1D("h_kine", "kine", 100, 0,1000);
  m_h_kine->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_kine->GetName(), m_h_kine));

  m_h_edep =  new TH1D("h_edep", "edep", 100, 0,1000);
  m_h_edep->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_edep->GetName(), m_h_edep));

  m_h_stepX =  new TH1D("h_stepX", "stepX", 100, 0,1000);
  m_h_stepX->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_stepX->GetName(), m_h_stepX));

  m_h_stepY =  new TH1D("h_stepY", "stepY", 100, 0,1000);
  m_h_stepY->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_stepY->GetName(), m_h_stepY));

  m_h_stepZ =  new TH1D("h_stepZ", "stepZ", 100, 0,1000);
  m_h_stepZ->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_stepZ->GetName(), m_h_stepZ));

  m_h_stationID =  new TH1D("h_stationID", "stationID", 50, 0,50);
  m_h_stationID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_stationID->GetName(), m_h_stationID));

  m_h_detID =  new TH1D("h_detID", "detID", 50, 0,50);
  m_h_detID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_detID->GetName(), m_h_detID));

  m_h_pixelRow =  new TH1D("h_pixelRow", "pixelRow", 20, 0,20);
  m_h_pixelRow->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_pixelRow->GetName(), m_h_pixelRow));

  m_h_pixelCol =  new TH1D("h_pixelCol", "pixelCol", 20, 0,20);
  m_h_pixelCol->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path+m_h_pixelCol->GetName(), m_h_pixelCol));
  
  /** now add branches and leaves to the tree */
  m_tree = new TTree("AFP","AFP");
  std::string fullNtupleName =  "/" + m_ntupleFileName + "/";
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));

  if (m_tree){
    m_tree->Branch("hitID", &m_hitID);
    m_tree->Branch("pdgID", &m_pdgID);
    m_tree->Branch("trackID", &m_trackID);
    m_tree->Branch("kine", &m_kine);
    m_tree->Branch("edep", &m_edep);
    m_tree->Branch("stepX", &m_stepX);
    m_tree->Branch("stepY", &m_stepY);
    m_tree->Branch("stepZ", &m_stepZ);
    m_tree->Branch("stationID", &m_stationID);
    m_tree->Branch("detID", &m_detID);
    m_tree->Branch("pixelRow", &m_pixelRow);
    m_tree->Branch("pixelCol", &m_pixelCol);  
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  ATH_MSG_INFO("Exiting AFPHitAnalysis::initialize()");
  
  return StatusCode::SUCCESS;
}		 
  

StatusCode AFPHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In AFPHitAnalysis::execute()" );

  m_hitID->clear();
  m_pdgID->clear();
  m_trackID->clear();
  m_kine->clear();
  m_edep->clear();
  m_stepX->clear();
  m_stepY->clear();
  m_stepZ->clear();
  m_time->clear();
  m_stationID->clear();
  m_detID->clear();
  m_pixelRow->clear();
  m_pixelCol->clear();

  ATH_MSG_DEBUG( "AFPHitAnalysis tree branches cleared" );

  AFP_SIDSimHitConstIter hi;
  const DataHandle<AFP_SIDSimHitCollection> iter;
  CHECK( evtStore()->retrieve(iter,"AFP_SIDSimHitCollection") );

  ATH_MSG_DEBUG( "AFP_SIDSSimHitCollection retrieved" );
  
  for ( hi=(*iter).begin(); hi != (*iter).end(); ++hi ) {
    AFP_SIDSimHit ghit(*hi);
    m_h_hitID->Fill(ghit.m_nHitID);
    m_h_pdgID->Fill(ghit.m_nParticleEncoding);
    m_h_trackID->Fill(ghit.m_nTrackID);
    m_h_kine->Fill(ghit.m_fKineticEnergy);
    m_h_edep->Fill(ghit.m_fEnergyDeposit);
    m_h_stepX->Fill(ghit.m_fPostStepX-ghit.m_fPreStepX);
    m_h_stepY->Fill(ghit.m_fPostStepY-ghit.m_fPreStepY);
    m_h_stepX->Fill(ghit.m_fPostStepZ-ghit.m_fPreStepZ);
    m_h_time->Fill(ghit.m_fGlobalTime);
    m_h_stationID->Fill(ghit.m_nStationID);
    m_h_detID->Fill(ghit.m_nDetectorID);
    m_h_pixelRow->Fill(ghit.m_nPixelRow);
    m_h_pixelCol->Fill(ghit.m_nPixelCol);

    m_hitID->push_back(ghit.m_nHitID);
    m_pdgID->push_back(ghit.m_nParticleEncoding);
    m_trackID->push_back(ghit.m_nTrackID);
    m_kine->push_back(ghit.m_fKineticEnergy);
    m_edep->push_back(ghit.m_fEnergyDeposit);
    m_stepX->push_back(ghit.m_fPostStepX-ghit.m_fPreStepX);
    m_stepY->push_back(ghit.m_fPostStepY-ghit.m_fPreStepY);
    m_stepX->push_back(ghit.m_fPostStepZ-ghit.m_fPreStepZ);
    m_time->push_back(ghit.m_fGlobalTime);
    m_stationID->push_back(ghit.m_nStationID);
    m_detID->push_back(ghit.m_nDetectorID);
    m_pixelRow->push_back(ghit.m_nPixelRow);
    m_pixelCol->push_back(ghit.m_nPixelCol);
  }
   
  if (m_tree) m_tree->Fill();
  
  return StatusCode::SUCCESS;
}
