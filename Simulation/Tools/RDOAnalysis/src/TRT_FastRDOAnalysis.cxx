/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_FastRDOAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

TRT_FastRDOAnalysis::TRT_FastRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputKey("TRT_DriftCircles")
  , m_word(0)
  , m_driftTimeBin(0)
  , m_trailEdge(0)
  , m_highLevel(0)
  , m_firstBin(0)
  , m_lastBin(0)
  , m_timeOverThreshold(0)
  , m_rawDriftTime(0)
  , m_driftTimeValid(0)
  , m_noise(0)
  , m_brl_ec(0)
  , m_trtID(0)
  , m_surfType(0)
  , m_bndsType(0)
  , m_cntr_x(0)
  , m_cntr_y(0)
  , m_cntr_z(0)
  , m_cntr_r(0)
  , m_cntr_phi(0)
  , m_norm_x(0)
  , m_norm_y(0)
  , m_norm_z(0)
  , m_norm_r(0)
  , m_norm_phi(0)
  , m_strSurfType(0)
  , m_strBndsType(0)
  , m_strCntr_x(0)
  , m_strCntr_y(0)
  , m_strCntr_z(0)
  , m_strCntr_r(0)
  , m_strCntr_phi(0)
  , m_strNorm_x(0)
  , m_strNorm_y(0)
  , m_strNorm_z(0)
  , m_strNorm_r(0)
  , m_strNorm_phi(0)
  , m_strawN(0)
  , m_strawL(0)
  , m_strawD(0)
  , m_allStrCntr_x(0)
  , m_allStrCntr_y(0)
  , m_allStrCntr_z(0)
  , m_allStrCntr_r(0)
  , m_allStrCntr_phi(0)
  , m_allStrAxis_x(0)
  , m_allStrAxis_y(0)
  , m_allStrAxis_z(0)
  , m_allStrAxis_r(0)
  , m_allStrAxis_phi(0)

  , h_word(0)
  , h_driftTimeBin(0)
  , h_trailEdge(0)
  , h_highLevel(0)
  , h_firstBin(0)
  , h_lastBin(0)
  , h_timeOverThreshold(0)
  , h_rawDriftTime(0)
  , h_driftTimeValid(0)
  , h_noise(0)
  , h_brl_ec(0)
  , h_trtID(0)
  , h_surfType(0)
  , h_bndsType(0)
  , h_cntr_x(0)
  , h_cntr_y(0)
  , h_cntr_z(0)
  , h_cntr_r(0)
  , h_cntr_phi(0)
  , h_norm_x(0)
  , h_norm_y(0)
  , h_norm_z(0)
  , h_norm_r(0)
  , h_norm_phi(0)
  , h_strSurfType(0)
  , h_strBndsType(0)
  , h_strCntr_x(0)
  , h_strCntr_y(0)
  , h_strCntr_z(0)
  , h_strCntr_r(0)
  , h_strCntr_phi(0)
  , h_strNorm_x(0)
  , h_strNorm_y(0)
  , h_strNorm_z(0)
  , h_strNorm_r(0)
  , h_strNorm_phi(0)
  , h_strawN(0)
  , h_strawL(0)
  , h_strawD(0)
  , h_allStrCntr_x(0)
  , h_allStrCntr_y(0)
  , h_allStrCntr_z(0)
  , h_allStrCntr_r(0)
  , h_allStrCntr_phi(0)
  , h_allStrAxis_x(0)
  , h_allStrAxis_y(0)
  , h_allStrAxis_z(0)
  , h_allStrAxis_r(0)
  , h_allStrAxis_phi(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/TRT_FastRDOAnalysis/")
  , m_ntupleTreeName("/TRT_FastRDOAna")
  , m_path("/TRT_FastRDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputKey", m_inputKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode TRT_FastRDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing TRT_FastRDOAnalysis" );

  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputKey.initialize() );

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "TRT_FastRDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    // TREE BRANCHES
    // TRT_DriftCircle
    m_tree->Branch("word", &m_word);
    m_tree->Branch("driftTimeBin", &m_driftTimeBin);
    m_tree->Branch("trailEdge", &m_trailEdge);
    m_tree->Branch("highLevel", &m_highLevel);
    m_tree->Branch("firstBin", &m_firstBin);
    m_tree->Branch("lastBin", &m_lastBin);
    m_tree->Branch("timeOverThreshold", &m_timeOverThreshold);
    m_tree->Branch("rawDriftTime", &m_rawDriftTime);
    m_tree->Branch("driftTimeValid", &m_driftTimeValid);
    m_tree->Branch("noise", &m_noise);
    // TRT_BaseElement
    m_tree->Branch("brl_ec", &m_brl_ec);
    m_tree->Branch("trtID", &m_trtID);
    m_tree->Branch("surfType", &m_surfType);
    m_tree->Branch("bndsType", &m_bndsType);
    m_tree->Branch("cntr_x", &m_cntr_x);
    m_tree->Branch("cntr_y", &m_cntr_y);
    m_tree->Branch("cntr_z", &m_cntr_z);
    m_tree->Branch("cntr_r", &m_cntr_r);
    m_tree->Branch("cntr_phi", &m_cntr_phi);
    m_tree->Branch("norm_x", &m_norm_x);
    m_tree->Branch("norm_y", &m_norm_y);
    m_tree->Branch("norm_z", &m_norm_z);
    m_tree->Branch("norm_r", &m_norm_r);
    m_tree->Branch("norm_phi", &m_norm_phi);
    m_tree->Branch("strSurfType", &m_strSurfType);
    m_tree->Branch("strBndsType", &m_strBndsType);
    m_tree->Branch("strCntr_x", &m_strCntr_x);
    m_tree->Branch("strCntr_y", &m_strCntr_y);
    m_tree->Branch("strCntr_z", &m_strCntr_z);
    m_tree->Branch("strCntr_r", &m_strCntr_r);
    m_tree->Branch("strCntr_phi", &m_strCntr_phi);
    m_tree->Branch("strNorm_x", &m_strNorm_x);
    m_tree->Branch("strNorm_y", &m_strNorm_y);
    m_tree->Branch("strNorm_z", &m_strNorm_z);
    m_tree->Branch("strNorm_r", &m_strNorm_r);
    m_tree->Branch("strNorm_phi", &m_strNorm_phi);
    m_tree->Branch("strawN", &m_strawN);
    m_tree->Branch("strawL", &m_strawL);
    m_tree->Branch("strawD", &m_strawD);
    m_tree->Branch("allStrCntr_x", &m_allStrCntr_x);
    m_tree->Branch("allStrCntr_y", &m_allStrCntr_y);
    m_tree->Branch("allStrCntr_z", &m_allStrCntr_z);
    m_tree->Branch("allStrCntr_r", &m_allStrCntr_r);
    m_tree->Branch("allStrCntr_phi", &m_allStrCntr_phi);
    m_tree->Branch("allStrAxis_x", &m_allStrAxis_x);
    m_tree->Branch("allStrAxis_y", &m_allStrAxis_y);
    m_tree->Branch("allStrAxis_z", &m_allStrAxis_z);
    m_tree->Branch("allStrAxis_r", &m_allStrAxis_r);
    m_tree->Branch("allStrAxis_phi", &m_allStrAxis_phi);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  // HISTOGRAMS
  h_word = new TH1F("h_word", "word", 100, 0, 7e7);
  h_word->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_word->GetName(), h_word));

  h_driftTimeBin = new TH1F("h_driftTimeBin", "driftTimeBin", 100, 0, 12);
  h_driftTimeBin->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_driftTimeBin->GetName(), h_driftTimeBin));

  h_trailEdge = new TH1F("h_trailEdge", "trailEdge", 100, 0, 16);
  h_trailEdge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_trailEdge->GetName(), h_trailEdge));

  h_highLevel = new TH1F("h_highLevel", "highLevel", 100, 0, 2);
  h_highLevel->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_highLevel->GetName(), h_highLevel));

  h_firstBin = new TH1F("h_firstBin", "firstBin", 100, 0, 2);
  h_firstBin->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_firstBin->GetName(), h_firstBin));

  h_lastBin = new TH1F("h_lastBin", "lastBin", 100, 0, 2);
  h_lastBin->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_lastBin->GetName(), h_lastBin));

  h_timeOverThreshold = new TH1F("h_timeOverThreshold", "timeOverThreshold", 100, 0, 255);
  h_timeOverThreshold->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_timeOverThreshold->GetName(), h_timeOverThreshold));

  h_rawDriftTime = new TH1F("h_rawDriftTime", "rawDriftTime", 100, 0, 35);
  h_rawDriftTime->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rawDriftTime->GetName(), h_rawDriftTime));

  h_driftTimeValid = new TH1F("h_driftTimeValid", "driftTimeValid", 100, 0, 2);
  h_driftTimeValid->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_driftTimeValid->GetName(), h_driftTimeValid));

  h_noise = new TH1F("h_noise", "noise", 100, 0, 2);
  h_noise->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_noise->GetName(), h_noise));

  h_brl_ec = new TH1F("h_brl_ec", "brl_ec", 100, 0, 2);
  h_brl_ec->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brl_ec->GetName(), h_brl_ec));

  h_trtID = new TH1F("h_trtID", "trtID", 100, 0, 2e18);
  h_trtID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_trtID->GetName(), h_trtID));

  h_surfType = new TH1F("h_surfType", "surfType", 100, 0, 7);
  h_surfType->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_surfType->GetName(), h_surfType));

  h_bndsType = new TH1F("h_bndsType", "bndsType", 100, 0, 7);
  h_bndsType->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_bndsType->GetName(), h_bndsType));

  h_cntr_x = new TH1F("h_cntr_x", "cntr_x", 100, -1250, 1250);
  h_cntr_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_cntr_x->GetName(), h_cntr_x));

  h_cntr_y = new TH1F("h_cntr_y", "cntr_y", 100, -1250, 1250);
  h_cntr_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_cntr_y->GetName(), h_cntr_y));

  h_cntr_z = new TH1F("h_cntr_z", "cntr_z", 100, -3000, 3000);
  h_cntr_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_cntr_z->GetName(), h_cntr_z));

  h_cntr_r = new TH1F("h_cntr_r", "cntr_r", 100, 0, 1250);
  h_cntr_r->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_cntr_r->GetName(), h_cntr_r));

  h_cntr_phi = new TH1F("h_cntr_phi", "cntr_phi", 100, -3.5, 3.5);
  h_cntr_phi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_cntr_phi->GetName(), h_cntr_phi));

  h_norm_x = new TH1F("h_norm_x", "norm_x", 100, -1.25, 1.25);
  h_norm_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_norm_x->GetName(), h_norm_x));

  h_norm_y = new TH1F("h_norm_y", "norm_y", 100, -1.25, 1.25);
  h_norm_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_norm_y->GetName(), h_norm_y));

  h_norm_z = new TH1F("h_norm_z", "norm_z", 100, -1, 1);
  h_norm_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_norm_z->GetName(), h_norm_z));

  h_norm_r = new TH1F("h_norm_r", "norm_r", 100, 0, 1);
  h_norm_r->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_norm_r->GetName(), h_norm_r));

  h_norm_phi = new TH1F("h_norm_phi", "norm_phi", 100, -3.5, 3.5);
  h_norm_phi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_norm_phi->GetName(), h_norm_phi));

  h_strSurfType = new TH1F("h_strSurfType", "strSurfType", 100, 0, 7);
  h_strSurfType->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strSurfType->GetName(), h_strSurfType));

  h_strBndsType = new TH1F("h_strBndsType", "strBndsType", 100, 0, 7);
  h_strBndsType->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strBndsType->GetName(), h_strBndsType));

  h_strCntr_x = new TH1F("h_strCntr_x", "strCntr_x", 100, -1250, 1250);
  h_strCntr_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strCntr_x->GetName(), h_strCntr_x));

  h_strCntr_y = new TH1F("h_strCntr_y", "strCntr_y", 100, -1250, 1250);
  h_strCntr_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strCntr_y->GetName(), h_strCntr_y));

  h_strCntr_z = new TH1F("h_strCntr_z", "strCntr_z", 100, -3000, 3000);
  h_strCntr_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strCntr_z->GetName(), h_strCntr_z));

  h_strCntr_r = new TH1F("h_strCntr_r", "strCntr_r", 100, 0, 1250);
  h_strCntr_r->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strCntr_r->GetName(), h_strCntr_r));

  h_strCntr_phi = new TH1F("h_strCntr_phi", "strCntr_phi", 100, -3.5, 3.5);
  h_strCntr_phi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strCntr_phi->GetName(), h_strCntr_phi));

  h_strNorm_x = new TH1F("h_strNorm_x", "strNorm_x", 100, -1.25, 1.25);
  h_strNorm_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strNorm_x->GetName(), h_strNorm_x));

  h_strNorm_y = new TH1F("h_strNorm_y", "strNorm_y", 100, -1.25, 1.25);
  h_strNorm_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strNorm_y->GetName(), h_strNorm_y));

  h_strNorm_z = new TH1F("h_strNorm_z", "strNorm_z", 100, -1, 1);
  h_strNorm_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strNorm_z->GetName(), h_strNorm_z));

  h_strNorm_r = new TH1F("h_strNorm_r", "strNorm_r", 100, 0, 1);
  h_strNorm_r->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strNorm_r->GetName(), h_strNorm_r));

  h_strNorm_phi = new TH1F("h_strNorm_phi", "strNorm_phi", 100, -3.5, 3.5);
  h_strNorm_phi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strNorm_phi->GetName(), h_strNorm_phi));

  h_strawN = new TH1F("h_strawN", "strawN", 100, 0, 30);
  h_strawN->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strawN->GetName(), h_strawN));

  h_strawL = new TH1F("h_strawL", "strawL", 100, 0, 750);
  h_strawL->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strawL->GetName(), h_strawL));

  h_strawD = new TH1F("h_strawD", "strawD", 100, -2, 2);
  h_strawD->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strawD->GetName(), h_strawD));

  h_allStrCntr_x = new TH1F("h_allStrCntr_x", "allStrCntr_x", 100, -1250, 1250);
  h_allStrCntr_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrCntr_x->GetName(), h_allStrCntr_x));

  h_allStrCntr_y = new TH1F("h_allStrCntr_y", "allStrCntr_y", 100, -1250, 1250);
  h_allStrCntr_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrCntr_y->GetName(), h_allStrCntr_y));

  h_allStrCntr_z = new TH1F("h_allStrCntr_z", "allStrCntr_z", 100, -3000, 3000);
  h_allStrCntr_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrCntr_z->GetName(), h_allStrCntr_z));

  h_allStrCntr_r = new TH1F("h_allStrCntr_r", "allStrCntr_r", 100, 0, 1250);
  h_allStrCntr_r->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrCntr_r->GetName(), h_allStrCntr_r));

  h_allStrCntr_phi = new TH1F("h_allStrCntr_phi", "allStrCntr_phi", 100, -3.5, 3.5);
  h_allStrCntr_phi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrCntr_phi->GetName(), h_allStrCntr_phi));

  h_allStrAxis_x = new TH1F("h_allStrAxis_x", "allStrAxis_x", 100, -1.25, 1.25);
  h_allStrAxis_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrAxis_x->GetName(), h_allStrAxis_x));

  h_allStrAxis_y = new TH1F("h_allStrAxis_y", "allStrAxis_y", 100, -1.25, 1.25);
  h_allStrAxis_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrAxis_y->GetName(), h_allStrAxis_y));

  h_allStrAxis_z = new TH1F("h_allStrAxis_z", "allStrAxis_z", 100, -1, 1);
  h_allStrAxis_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrAxis_z->GetName(), h_allStrAxis_z));

  h_allStrAxis_r = new TH1F("h_allStrAxis_r", "allStrAxis_r", 100, 0, 1);
  h_allStrAxis_r->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrAxis_r->GetName(), h_allStrAxis_r));

  h_allStrAxis_phi = new TH1F("h_allStrAxis_phi", "allStrAxis_phi", 100, -3.5, 3.5);
  h_allStrAxis_phi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_allStrAxis_phi->GetName(), h_allStrAxis_phi));

  return StatusCode::SUCCESS;
}

StatusCode TRT_FastRDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In TRT_FastRDOAnalysis::execute()" );

  // clear branches
  m_word->clear();
  m_driftTimeBin->clear();
  m_highLevel->clear();
  m_firstBin->clear();
  m_lastBin->clear();
  m_timeOverThreshold->clear();
  m_rawDriftTime->clear();
  m_driftTimeValid->clear();
  m_noise->clear();

  m_brl_ec->clear();
  m_trtID->clear();
  m_surfType->clear();
  m_bndsType->clear();
  m_cntr_x->clear();
  m_cntr_y->clear();
  m_cntr_z->clear();
  m_cntr_r->clear();
  m_cntr_phi->clear();
  m_norm_x->clear();
  m_norm_y->clear();
  m_norm_z->clear();
  m_norm_r->clear();
  m_norm_phi->clear();
  m_strSurfType->clear();
  m_strBndsType->clear();
  m_strCntr_x->clear();
  m_strCntr_y->clear();
  m_strCntr_z->clear();
  m_strCntr_r->clear();
  m_strCntr_phi->clear();
  m_strNorm_x->clear();
  m_strNorm_y->clear();
  m_strNorm_z->clear();
  m_strNorm_r->clear();
  m_strNorm_phi->clear();
  m_strawN->clear();
  m_strawL->clear();
  m_strawD->clear();
  m_allStrCntr_x->clear();
  m_allStrCntr_y->clear();
  m_allStrCntr_z->clear();
  m_allStrCntr_r->clear();
  m_allStrCntr_phi->clear();
  m_allStrAxis_x->clear();
  m_allStrAxis_y->clear();
  m_allStrAxis_z->clear();
  m_allStrAxis_r->clear();
  m_allStrAxis_phi->clear();

  // get containers -- fill branches + histos
  SG::ReadHandle<InDet::TRT_DriftCircleContainer> p_trtDC_cont (m_inputKey);
  if(p_trtDC_cont.isValid()) {
    // loop over cluster container
    InDet::TRT_DriftCircleContainer::const_iterator trtDCcont_itr(p_trtDC_cont->begin());
    const InDet::TRT_DriftCircleContainer::const_iterator trtDCcont_end(p_trtDC_cont->end());
    for ( ; trtDCcont_itr != trtDCcont_end; ++trtDCcont_itr ) {
      const InDet::TRT_DriftCircleCollection* p_trtDC_coll(*trtDCcont_itr);
      InDet::TRT_DriftCircleCollection::const_iterator trtDC_itr(p_trtDC_coll->begin());
      const InDet::TRT_DriftCircleCollection::const_iterator trtDC_end(p_trtDC_coll->end());
      for ( ; trtDC_itr != trtDC_end; ++trtDC_itr ) {
        // TRT_DriftCircle
        const unsigned int word((*trtDC_itr)->getWord());
        const int driftTimeBin((*trtDC_itr)->driftTimeBin());
        const int trailEdge((*trtDC_itr)->trailingEdge());
        const bool HL((*trtDC_itr)->highLevel());
        const bool firstBin((*trtDC_itr)->firstBinHigh());
        const bool lastBin((*trtDC_itr)->lastBinHigh());
        const double ToT((*trtDC_itr)->timeOverThreshold());
        const double rawDriftTime((*trtDC_itr)->rawDriftTime());
        const bool driftTimeValid((*trtDC_itr)->driftTimeValid());
        const bool noise((*trtDC_itr)->isNoise());

        m_word->push_back(word);
        m_driftTimeBin->push_back(driftTimeBin);
        m_trailEdge->push_back(trailEdge);
        m_highLevel->push_back(HL);
        m_firstBin->push_back(firstBin);
        m_lastBin->push_back(lastBin);
        m_timeOverThreshold->push_back(ToT);
        m_rawDriftTime->push_back(rawDriftTime);
        m_driftTimeValid->push_back(driftTimeValid);
        m_noise->push_back(noise);

        h_word->Fill(word);
        h_driftTimeBin->Fill(driftTimeBin);
        h_trailEdge->Fill(trailEdge);
        h_highLevel->Fill(HL);
        h_firstBin->Fill(firstBin);
        h_lastBin->Fill(lastBin);
        h_timeOverThreshold->Fill(ToT);
        h_rawDriftTime->Fill(rawDriftTime);
        h_driftTimeValid->Fill(driftTimeValid);
        h_noise->Fill(noise);

        // ---------------
        // TRT_BaseElement
        // ---------------
        const InDetDD::TRT_BaseElement* detElement((*trtDC_itr)->detectorElement());
        // type info --> BARREL or ENDCAP
        const int brl_ec((*detElement).type());
        const Identifier trtID((*detElement).identify());
        const unsigned long long trtID_int = trtID.get_compact();

        // Surface & Tracking info
        // -- element surface (straw layer)
        const Trk::Surface& surf((*detElement).surface());
        const int surfType(surf.type());

        const Trk::SurfaceBounds& bnds((*detElement).bounds());
        const int bndsType(bnds.type());

        const Amg::Vector3D& cntr((*detElement).center());
        const float cntr_x(cntr.x());
        const float cntr_y(cntr.y());
        const float cntr_z(cntr.z());
        const float cntr_r(cntr.perp());
        const float cntr_phi(cntr.phi());

        const Amg::Vector3D& norm((*detElement).normal());
        const float norm_x(norm.x());
        const float norm_y(norm.y());
        const float norm_z(norm.z());
        const float norm_r(norm.perp());
        const float norm_phi(norm.phi());

        // -- straw surface (first straw in layer?)
        const Trk::Surface& strSurf((*detElement).surface(trtID));
        const int strSurfType(strSurf.type());

        const Trk::SurfaceBounds& strBnds((*detElement).bounds(trtID));
        const int strBndsType(strBnds.type());

        const Amg::Vector3D& strCntr((*detElement).center(trtID));
        const float strCntr_x(strCntr.x());
        const float strCntr_y(strCntr.y());
        const float strCntr_z(strCntr.z());
        const float strCntr_r(strCntr.perp());
        const float strCntr_phi(strCntr.phi());

        const Amg::Vector3D& strNorm((*detElement).normal(trtID));
        const float strNorm_x(strNorm.x());
        const float strNorm_y(strNorm.y());
        const float strNorm_z(strNorm.z());
        const float strNorm_r(strNorm.perp());
        const float strNorm_phi(strNorm.phi());

        const unsigned int strawN((*detElement).nStraws());
        const double& strawL((*detElement).strawLength());
        const int strawD((*detElement).strawDirection());

        // all straws in layer
        for (unsigned int i = 0; i != strawN; ++i) {
          const Amg::Vector3D& strawCenter((*detElement).strawCenter(i));
          const float strawCenter_x(strawCenter.x());
          const float strawCenter_y(strawCenter.y());
          const float strawCenter_z(strawCenter.z());
          const float strawCenter_r(strawCenter.perp());
          const float strawCenter_phi(strawCenter.phi());

          const Amg::Vector3D& strawAxis((*detElement).strawAxis(i));
          const float strawAxis_x(strawAxis.x());
          const float strawAxis_y(strawAxis.y());
          const float strawAxis_z(strawAxis.z());
          const float strawAxis_r(strawAxis.perp());
          const float strawAxis_phi(strawAxis.phi());

          m_allStrCntr_x->push_back(strawCenter_x);
          m_allStrCntr_y->push_back(strawCenter_y);
          m_allStrCntr_z->push_back(strawCenter_z);
          m_allStrCntr_r->push_back(strawCenter_r);
          m_allStrCntr_phi->push_back(strawCenter_phi);

          m_allStrAxis_x->push_back(strawAxis_x);
          m_allStrAxis_y->push_back(strawAxis_y);
          m_allStrAxis_z->push_back(strawAxis_z);
          m_allStrAxis_r->push_back(strawAxis_r);
          m_allStrAxis_phi->push_back(strawAxis_phi);

          h_allStrCntr_x->Fill(strawCenter_x);
          h_allStrCntr_y->Fill(strawCenter_y);
          h_allStrCntr_z->Fill(strawCenter_z);
          h_allStrCntr_r->Fill(strawCenter_r);
          h_allStrCntr_phi->Fill(strawCenter_phi);

          h_allStrAxis_x->Fill(strawAxis_x);
          h_allStrAxis_y->Fill(strawAxis_y);
          h_allStrAxis_z->Fill(strawAxis_z);
          h_allStrAxis_r->Fill(strawAxis_r);
          h_allStrAxis_phi->Fill(strawAxis_phi);
        }

        m_brl_ec->push_back(brl_ec);
        m_trtID->push_back(trtID_int);
        m_surfType->push_back(surfType);
        m_bndsType->push_back(bndsType);
        m_cntr_x->push_back(cntr_x);
        m_cntr_y->push_back(cntr_y);
        m_cntr_z->push_back(cntr_z);
        m_cntr_r->push_back(cntr_r);
        m_cntr_phi->push_back(cntr_phi);
        m_norm_x->push_back(norm_x);
        m_norm_y->push_back(norm_y);
        m_norm_z->push_back(norm_z);
        m_norm_r->push_back(norm_r);
        m_norm_phi->push_back(norm_phi);
        m_strSurfType->push_back(strSurfType);
        m_strBndsType->push_back(strBndsType);
        m_strCntr_x->push_back(strCntr_x);
        m_strCntr_y->push_back(strCntr_y);
        m_strCntr_z->push_back(strCntr_z);
        m_strCntr_r->push_back(strCntr_r);
        m_strCntr_phi->push_back(strCntr_phi);
        m_strNorm_x->push_back(strNorm_x);
        m_strNorm_y->push_back(strNorm_y);
        m_strNorm_z->push_back(strNorm_z);
        m_strNorm_r->push_back(strNorm_r);
        m_strNorm_phi->push_back(strNorm_phi);
        m_strawN->push_back(strawN);
        m_strawL->push_back(strawL);
        m_strawD->push_back(strawD);

        h_brl_ec->Fill(brl_ec);
        h_trtID->Fill(trtID_int);
        h_surfType->Fill(surfType);
        h_bndsType->Fill(bndsType);
        h_cntr_x->Fill(cntr_x);
        h_cntr_y->Fill(cntr_y);
        h_cntr_z->Fill(cntr_z);
        h_cntr_r->Fill(cntr_r);
        h_cntr_phi->Fill(cntr_phi);
        h_norm_x->Fill(norm_x);
        h_norm_y->Fill(norm_y);
        h_norm_z->Fill(norm_z);
        h_norm_r->Fill(norm_r);
        h_norm_phi->Fill(norm_phi);
        h_strSurfType->Fill(strSurfType);
        h_strBndsType->Fill(strBndsType);
        h_strCntr_x->Fill(strCntr_x);
        h_strCntr_y->Fill(strCntr_y);
        h_strCntr_z->Fill(strCntr_z);
        h_strCntr_r->Fill(strCntr_r);
        h_strCntr_phi->Fill(strCntr_phi);
        h_strNorm_x->Fill(strNorm_x);
        h_strNorm_y->Fill(strNorm_y);
        h_strNorm_z->Fill(strNorm_z);
        h_strNorm_r->Fill(strNorm_r);
        h_strNorm_phi->Fill(strNorm_phi);
        h_strawN->Fill(strawN);
        h_strawL->Fill(strawL);
        h_strawD->Fill(strawD);

      }
    }
  }

  if (m_tree) {
    m_tree->Fill();
  }

  return StatusCode::SUCCESS;
}

StatusCode TRT_FastRDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
