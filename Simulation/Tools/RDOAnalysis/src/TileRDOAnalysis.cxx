/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TileRDOAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TileEvent/TileRawChannelCollection.h"
#include "TileEvent/TileDigitsCollection.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

TileRDOAnalysis::TileRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputRawChKey("TileRawChannelCnt")
  , m_inputMuRcvRawChKey("TileRawChannelCnt")
  , m_inputMuRcvKey("TileMuRcvCnt")
  , m_inputMBTS_TTL1Key("TileTTL1MBTS")
  , m_inputTileTTL1Key("TileTTL1Cnt")
  , m_inputL2Key("TileL2Cnt")
  , m_inputDigitsFltKey("TileDigitsFlt")
  , m_inputDigitsMuRcvKey("MuRcvDigitsCnt")
  , m_adcID(0)
  , m_pmtID(0)
  , m_cellID(0)
  , m_ttID(0)
  , m_mtID(0)
  , m_fragID(0)
  , m_rawAmp(0)
  , m_rawTime(0)
  , m_rawQual(0)
  , m_rawPed(0)
  , m_adcID_mu(0)
  , m_pmtID_mu(0)
  , m_cellID_mu(0)
  , m_ttID_mu(0)
  , m_mtID_mu(0)
  , m_fragID_mu(0)
  , m_rawAmp_mu(0)
  , m_rawTime_mu(0)
  , m_rawQual_mu(0)
  , m_rawPed_mu(0)
  , m_muRcvID(0)
  , m_muRcv_dec(0)
  , m_muRcv_thresh(0)
  , m_muRcv_energy(0)
  , m_muRcv_time(0)
  , m_ttl1MBTS_ID(0)
  , m_ttl1MBTS_digits(0)
  , m_ttl1_ID(0)
  , m_ttl1_digits(0)
  , m_L2ID(0)
  , m_L2val(0)
  , m_L2eta(0)
  , m_L2phi(0)
  , m_L2energyA(0)
  , m_L2energyBC(0)
  , m_L2energyD(0)
  , m_L2qual(0)
  , m_L2sumE(0)
  , m_fragSize(0)
  , m_fragBCID(0)
  , m_digits(0)
  , m_muFragSize(0)
  , m_muFragBCID(0)
  , m_muDigits(0)

  , m_h_adcID(0)
  , m_h_rawAmp(0)
  , m_h_rawTime(0)
  , m_h_rawQual(0)
  , m_h_rawPed(0)
  , m_h_adcID_mu(0)
  , m_h_rawAmp_mu(0)
  , m_h_rawTime_mu(0)
  , m_h_rawQual_mu(0)
  , m_h_rawPed_mu(0)
  , m_h_muRcvID(0)
  , m_h_muRcv_dec(0)
  , m_h_muRcv_thresh(0)
  , m_h_muRcv_energy(0)
  , m_h_muRcv_time(0)
  , m_h_ttl1MBTS_ID(0)
  , m_h_ttl1MBTS_digits(0)
  , m_h_ttl1_ID(0)
  , m_h_ttl1_digits(0)
  , m_h_L2ID(0)
  , m_h_L2val(0)
  , m_h_L2eta(0)
  , m_h_L2phi(0)
  , m_h_L2energyA(0)
  , m_h_L2energyBC(0)
  , m_h_L2energyD(0)
  , m_h_L2qual(0)
  , m_h_L2sumE(0)
  , m_h_digits(0)
  , m_h_muDigits(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/TileRDOAnalysis/")
  , m_ntupleTreeName("TileRDOAna")
  , m_path("/TileRDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputRawChKey", m_inputRawChKey);
  declareProperty("InputMuRcvRawChKey", m_inputMuRcvRawChKey);
  declareProperty("InputMuRcvKey", m_inputMuRcvKey);
  declareProperty("InputMBTS_TTL1Key", m_inputMBTS_TTL1Key);
  declareProperty("InputTileTTL1Key", m_inputTileTTL1Key);
  declareProperty("InputL2Key", m_inputL2Key);
  declareProperty("InputDigitsFltKey", m_inputDigitsFltKey);
  declareProperty("InputDigitsMuRcvKey", m_inputDigitsMuRcvKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode TileRDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing TileRDOAnalysis" );


  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputRawChKey.initialize() );
  ATH_CHECK( m_inputMuRcvRawChKey.initialize() );
  ATH_CHECK( m_inputMuRcvKey.initialize() );
  ATH_CHECK( m_inputMBTS_TTL1Key.initialize() );
  ATH_CHECK( m_inputTileTTL1Key.initialize() );
  ATH_CHECK( m_inputL2Key.initialize() );
  ATH_CHECK( m_inputDigitsFltKey.initialize() );
  ATH_CHECK( m_inputDigitsMuRcvKey.initialize() );

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "TileRDOAna");
  std::string fullNtupleName = "/" + m_ntupleFileName + "/" + m_ntupleDirName + "/" + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    m_tree->Branch("adcID", &m_adcID);
    m_tree->Branch("pmtID", &m_pmtID);
    m_tree->Branch("cellID", &m_cellID);
    m_tree->Branch("ttID", &m_ttID);
    m_tree->Branch("mtID", &m_mtID);
    m_tree->Branch("fragID", &m_fragID);
    m_tree->Branch("rawAmp", &m_rawAmp);
    m_tree->Branch("rawTime", &m_rawTime);
    m_tree->Branch("rawQual", &m_rawQual);
    m_tree->Branch("rawPed", &m_rawPed);
    m_tree->Branch("adcID_mu", &m_adcID_mu);
    m_tree->Branch("pmtID_mu", &m_pmtID_mu);
    m_tree->Branch("cellID_mu", &m_cellID_mu);
    m_tree->Branch("ttID_mu", &m_ttID_mu);
    m_tree->Branch("mtID_mu", &m_mtID_mu);
    m_tree->Branch("fragID_mu", &m_fragID_mu);
    m_tree->Branch("rawAmp_mu", &m_rawAmp_mu);
    m_tree->Branch("rawTime_mu", &m_rawTime_mu);
    m_tree->Branch("rawQual_mu", &m_rawQual_mu);
    m_tree->Branch("rawPed_mu", &m_rawPed_mu);
    m_tree->Branch("muRcvID", &m_muRcvID);
    m_tree->Branch("muRcv_dec", &m_muRcv_dec);
    m_tree->Branch("muRcv_thresh", &m_muRcv_thresh);
    m_tree->Branch("muRcv_energy", &m_muRcv_energy);
    m_tree->Branch("muRcv_time", &m_muRcv_time);
    m_tree->Branch("ttl1MBTS_ID", &m_ttl1MBTS_ID);
    m_tree->Branch("ttl1MBTS_digits", &m_ttl1MBTS_digits);
    m_tree->Branch("ttl1_ID", &m_ttl1_ID);
    m_tree->Branch("ttl1_digits", &m_ttl1_digits);
    m_tree->Branch("L2ID", &m_L2ID);
    m_tree->Branch("L2val", &m_L2val);
    m_tree->Branch("L2eta", &m_L2eta);
    m_tree->Branch("L2phi", &m_L2phi);
    m_tree->Branch("L2energyA", &m_L2energyA);
    m_tree->Branch("L2energyBC", &m_L2energyBC);
    m_tree->Branch("L2energyD", &m_L2energyD);
    m_tree->Branch("L2qual", &m_L2qual);
    m_tree->Branch("L2sumE", &m_L2sumE);
    m_tree->Branch("fragSize", &m_fragSize);
    m_tree->Branch("fragBCID", &m_fragBCID);
    m_tree->Branch("digits", &m_digits);
    m_tree->Branch("muFragSize", &m_muFragSize);
    m_tree->Branch("muFragBCID", &m_muFragBCID);
    m_tree->Branch("muDigits", &m_muDigits);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  // HISTOGRAMS
  m_h_adcID = new TH1F("h_adcID", "adc ID", 100, 0, 9.25e18);
  m_h_adcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_adcID->GetName(), m_h_adcID));

  m_h_rawAmp = new TH1F("h_rawAmp", "Raw amplitude", 100, -1200, 1200);
  m_h_rawAmp->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawAmp->GetName(), m_h_rawAmp));

  m_h_rawTime = new TH1F("h_rawTime", "Raw time", 100, -90, 90);
  m_h_rawTime->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawTime->GetName(), m_h_rawTime));

  m_h_rawQual = new TH1F("h_rawQual", "Raw quality", 100, 0, 1100);
  m_h_rawQual->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawQual->GetName(), m_h_rawQual));

  m_h_rawPed = new TH1F("h_rawPed", "Raw pedestal", 100, 0, 2e5);
  m_h_rawPed->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawPed->GetName(), m_h_rawPed));

  m_h_adcID_mu = new TH1F("h_adcID_mu", "MuRcv adc ID", 100, 0, 9.25e18);
  m_h_adcID_mu->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_adcID_mu->GetName(), m_h_adcID_mu));

  m_h_rawAmp_mu = new TH1F("h_rawAmp_mu", "MuRcv raw amplitude", 100, -1000, 11000);
  m_h_rawAmp_mu->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawAmp_mu->GetName(), m_h_rawAmp_mu));

  m_h_rawTime_mu = new TH1F("h_rawTime_mu", "MuRcv raw time", 100, -90, 90);
  m_h_rawTime_mu->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawTime_mu->GetName(), m_h_rawTime_mu));

  m_h_rawQual_mu = new TH1F("h_rawQual_mu", "MuRcv raw quality", 100, 0, 8e34);
  m_h_rawQual_mu->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawQual_mu->GetName(), m_h_rawQual_mu));

  m_h_rawPed_mu = new TH1F("h_rawPed_mu", "MuRcv raw pedestal", 100, 0, 13);
  m_h_rawPed_mu->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rawPed_mu->GetName(), m_h_rawPed_mu));

  m_h_muRcvID = new TH1F("h_muRcvID", "Muon receiver object ID", 100, 0, 500);
  m_h_muRcvID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_muRcvID->GetName(), m_h_muRcvID));

  m_h_muRcv_dec = new TH1F("h_muRcv_dec", "Muon receiver object decision", 100, 0, 2);
  m_h_muRcv_dec->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_muRcv_dec->GetName(), m_h_muRcv_dec));

  m_h_muRcv_thresh = new TH1F("h_muRcv_thresh", "Muon receiver object threshold", 100, 0, 650);
  m_h_muRcv_thresh->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_muRcv_thresh->GetName(), m_h_muRcv_thresh));

  m_h_muRcv_energy = new TH1F("h_muRcv_energy", "Muon receiver object energy", 100, 0, 20000);
  m_h_muRcv_energy->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_muRcv_energy->GetName(), m_h_muRcv_energy));

  m_h_muRcv_time = new TH1F("h_muRcv_time", "Muon receiver object time", 100, -90, 90);
  m_h_muRcv_time->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_muRcv_time->GetName(), m_h_muRcv_time));

  m_h_ttl1MBTS_ID = new TH1F("h_ttl1MBTS_ID", "TTL1 MBTS ID", 100, 0, 9.25e18);
  m_h_ttl1MBTS_ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ttl1MBTS_ID->GetName(), m_h_ttl1MBTS_ID));

  m_h_ttl1MBTS_digits = new TH1F("h_ttl1MBTS_digits", "TTL1 MBTS digits", 100, 0, 2000);
  m_h_ttl1MBTS_digits->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ttl1MBTS_digits->GetName(), m_h_ttl1MBTS_digits));

  m_h_ttl1_ID = new TH1F("h_ttl1_ID", "TTL1 ID", 100, 0, 2e19);
  m_h_ttl1_ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ttl1_ID->GetName(), m_h_ttl1_ID));

  m_h_ttl1_digits = new TH1F("h_ttl1_digits", "TTL1 digits", 100, 0, 2000);
  m_h_ttl1_digits->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ttl1_digits->GetName(), m_h_ttl1_digits));

  m_h_L2ID = new TH1F("h_L2ID", "L2 ID", 100, 0, 2e19);
  m_h_L2ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2ID->GetName(), m_h_L2ID));

  m_h_L2val = new TH1F("h_L2val", "L2 data values", 100, 0, 100);
  m_h_L2val->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2val->GetName(), m_h_L2val));

  m_h_L2eta = new TH1F("h_L2eta", "L2 eta", 100, -1.5, 1.5);
  m_h_L2eta->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2eta->GetName(), m_h_L2eta));

  m_h_L2phi = new TH1F("h_L2phi", "L2 phi", 100, -3.5, 3.5);
  m_h_L2phi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2phi->GetName(), m_h_L2phi));

  m_h_L2energyA = new TH1F("h_L2energyA", "L2 energy in A cells", 100, 0, 12500);
  m_h_L2energyA->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2energyA->GetName(), m_h_L2energyA));

  m_h_L2energyBC = new TH1F("h_L2energyBC", "L2 energy in BC cells", 100, 0, 12500);
  m_h_L2energyBC->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2energyBC->GetName(), m_h_L2energyBC));

  m_h_L2energyD = new TH1F("h_L2energyD", "L2 energy in D cells", 100, 0, 12500);
  m_h_L2energyD->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2energyD->GetName(), m_h_L2energyD));

  m_h_L2qual = new TH1F("h_L2qual", "L2 quality", 100, 0, 2);
  m_h_L2qual->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2qual->GetName(), m_h_L2qual));

  m_h_L2sumE = new TH1F("h_L2sumE", "L2 energy sum", 100, 0, 2.25e5);
  m_h_L2sumE->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_L2sumE->GetName(), m_h_L2sumE));

  m_h_digits = new TH1F("h_digits", "Tile digits", 100, 0, 1100);
  m_h_digits->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_digits->GetName(), m_h_digits));

  m_h_muDigits = new TH1F("h_muDigits", "Tile muon receiver object digits", 100, 0, 150);
  m_h_muDigits->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_muDigits->GetName(), m_h_muDigits));



  return StatusCode::SUCCESS;
}

StatusCode TileRDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In TileRDOAnalysis::execute()" );

  m_adcID->clear();
  m_pmtID->clear();
  m_cellID->clear();
  m_ttID->clear();
  m_mtID->clear();
  m_fragID->clear();
  m_rawAmp->clear();
  m_rawTime->clear();
  m_rawQual->clear();
  m_rawPed->clear();
  m_adcID_mu->clear();
  m_pmtID_mu->clear();
  m_cellID_mu->clear();
  m_ttID_mu->clear();
  m_mtID_mu->clear();
  m_fragID_mu->clear();
  m_rawAmp_mu->clear();
  m_rawTime_mu->clear();
  m_rawQual_mu->clear();
  m_rawPed_mu->clear();
  m_muRcvID->clear();
  m_muRcv_dec->clear();
  m_muRcv_thresh->clear();
  m_muRcv_energy->clear();
  m_muRcv_time->clear();
  m_ttl1MBTS_ID->clear();
  m_ttl1MBTS_digits->clear();
  m_ttl1_ID->clear();
  m_ttl1_digits->clear();
  m_L2ID->clear();
  m_L2val->clear();
  m_L2eta->clear();
  m_L2phi->clear();
  m_L2energyA->clear();
  m_L2energyBC->clear();
  m_L2energyD->clear();
  m_L2qual->clear();
  m_L2sumE->clear();
  m_fragSize->clear();
  m_fragBCID->clear();
  m_digits->clear();
  m_muFragSize->clear();
  m_muFragBCID->clear();
  m_muDigits->clear();

  // Tile Raw Channels
  // Raw info (pulse height, time, quality) for in-time beam crossing in Tile

  SG::ReadHandle<TileRawChannelContainer> p_rawCont(m_inputRawChKey);
  if (p_rawCont.isValid()) {
    // loop over tile raw channels container
    TileRawChannelContainer::const_iterator rawCont_itr(p_rawCont->begin());
    const TileRawChannelContainer::const_iterator rawCont_end(p_rawCont->end());
    for ( ; rawCont_itr != rawCont_end; ++rawCont_itr ) {
      const TileRawDataCollection<TileRawChannel>* p_rawColl(*rawCont_itr);
      TileRawDataCollection<TileRawChannel>::const_iterator raw_itr(p_rawColl->begin());
      const TileRawDataCollection<TileRawChannel>::const_iterator raw_end(p_rawColl->end());
      for ( ; raw_itr != raw_end; ++raw_itr ) {
        const Identifier adcID((*raw_itr)->adc_ID());
        const Identifier pmtID((*raw_itr)->pmt_ID());
        const Identifier cellID((*raw_itr)->cell_ID());
        const Identifier ttID((*raw_itr)->tt_ID());
        const Identifier mtID((*raw_itr)->mt_ID());
        const int fragID((*raw_itr)->frag_ID());

        const unsigned long long adcID_int = adcID.get_compact();
        const unsigned long long pmtID_int = pmtID.get_compact();
        const unsigned long long cellID_int = cellID.get_compact();
        const unsigned long long ttID_int = ttID.get_compact();
        const unsigned long long mtID_int = mtID.get_compact();

        m_adcID->push_back(adcID_int);
        m_pmtID->push_back(pmtID_int);
        m_cellID->push_back(cellID_int);
        m_ttID->push_back(ttID_int);
        m_mtID->push_back(mtID_int);
        m_fragID->push_back(fragID);

        m_h_adcID->Fill(adcID_int);

        for (int ix = 0; ix != (*raw_itr)->size(); ++ix) {
          m_rawAmp->push_back((*raw_itr)->amplitude(ix)); // [ADC counts]
          m_h_rawAmp->Fill((*raw_itr)->amplitude(ix));
          m_rawQual->push_back((*raw_itr)->quality(ix)); // sampling distr.
          m_h_rawQual->Fill((*raw_itr)->quality(ix));
        }
        for (int jx = 0; jx != (*raw_itr)->sizeTime(); ++jx) {
          m_rawTime->push_back((*raw_itr)->time(jx)); // rel to triggering bunch
          m_h_rawTime->Fill((*raw_itr)->time(jx));
        }
        // // cannot find member 'sizeQuality()' --- using old header?
        // for (int kx = 0; kx != (*raw_itr)->sizeQuality(); ++kx) {
        //   m_rawQual->push_back((*raw_itr)->quality(kx)); // sampling distr.
        //   m_h_rawQual->Fill((*raw_itr)->quality(kx));
        // }
        m_rawPed->push_back((*raw_itr)->pedestal()); // reconstructed
        m_h_rawPed->Fill((*raw_itr)->pedestal());
      }
    }
  }

  // Muon Receiver Raw Channels
  SG::ReadHandle<TileRawChannelContainer> p_mu_rawCont(m_inputMuRcvRawChKey);
  if (p_mu_rawCont.isValid()) {
    // loop over muon receiver raw channels container
    TileRawChannelContainer::const_iterator muRawCont_itr(p_mu_rawCont->begin());
    const TileRawChannelContainer::const_iterator muRawCont_end(p_mu_rawCont->end());
    for ( ; muRawCont_itr != muRawCont_end; ++muRawCont_itr ) {
      const TileRawDataCollection<TileRawChannel>* p_mu_rawColl(*muRawCont_itr);
      TileRawDataCollection<TileRawChannel>::const_iterator muRaw_itr(p_mu_rawColl->begin());
      const TileRawDataCollection<TileRawChannel>::const_iterator muRaw_end(p_mu_rawColl->end());
      for ( ; muRaw_itr != muRaw_end; ++muRaw_itr ) {
        const Identifier adcID_mu((*muRaw_itr)->adc_ID());
        const Identifier pmtID_mu((*muRaw_itr)->pmt_ID());
        const Identifier cellID_mu((*muRaw_itr)->cell_ID());
        const Identifier ttID_mu((*muRaw_itr)->tt_ID());
        const Identifier mtID_mu((*muRaw_itr)->mt_ID());
        const int fragID_mu((*muRaw_itr)->frag_ID());

        const unsigned long long adcID_mu_int = adcID_mu.get_compact();
        const unsigned long long pmtID_mu_int = pmtID_mu.get_compact();
        const unsigned long long cellID_mu_int = cellID_mu.get_compact();
        const unsigned long long ttID_mu_int = ttID_mu.get_compact();
        const unsigned long long mtID_mu_int = mtID_mu.get_compact();

        m_adcID_mu->push_back(adcID_mu_int);
        m_pmtID_mu->push_back(pmtID_mu_int);
        m_cellID_mu->push_back(cellID_mu_int);
        m_ttID_mu->push_back(ttID_mu_int);
        m_mtID_mu->push_back(mtID_mu_int);
        m_fragID_mu->push_back(fragID_mu);

        m_h_adcID_mu->Fill(adcID_mu_int);

        for (int lx = 0; lx != (*muRaw_itr)->size(); ++lx){
          m_rawAmp_mu->push_back((*muRaw_itr)->amplitude(lx));
          m_h_rawAmp_mu->Fill((*muRaw_itr)->amplitude(lx));
          m_rawQual_mu->push_back((*muRaw_itr)->quality(lx));
          m_h_rawQual_mu->Fill((*muRaw_itr)->quality(lx));
        }
        for (int mx = 0; mx != (*muRaw_itr)->sizeTime(); ++mx) {
          m_rawTime_mu->push_back((*muRaw_itr)->time(mx));
          m_h_rawTime_mu->Fill((*muRaw_itr)->time(mx));
        }
        // // cannot find member 'sizeQuality()' --- using old header?
        // for (int nx = 0; nx != (*muRaw_itr)->sizeQuality(); ++nx) {
        //   m_rawQual_mu->push_back((*muRaw_itr)->quality(nx));
        //   m_h_rawQual_mu->Fill((*muRaw_itr)->quality(nx));
        // }
        m_rawPed_mu->push_back((*muRaw_itr)->pedestal());
        m_h_rawPed_mu->Fill((*muRaw_itr)->pedestal());
      }
    }
  }


  // Tile Container - TileMuonReceiverContainer
  SG::ReadHandle<TileMuonReceiverContainer> p_muRcv_cont(m_inputMuRcvKey);
  if (p_muRcv_cont.isValid()) {
    // loop over muon receiver container
    TileMuonReceiverContainer::const_iterator muRcv_itr(p_muRcv_cont->begin());
    const TileMuonReceiverContainer::const_iterator muRcv_end(p_muRcv_cont->end());
    for ( ; muRcv_itr != muRcv_end; ++muRcv_itr ) {
      const int muRcvID((*muRcv_itr)->GetID());
      const std::vector<bool>& dec_vec = (*muRcv_itr)->GetDecision();
      const std::vector<float>& thresh_vec = (*muRcv_itr)->GetThresholds();
      const std::vector<float>& ene_vec = (*muRcv_itr)->GetEne();
      const std::vector<float>& time_vec = (*muRcv_itr)->GetTime();

      m_muRcvID->push_back(muRcvID);

      for (std::vector<bool>::size_type i = 0; i != dec_vec.size(); ++i) {
        m_muRcv_dec->push_back(dec_vec.at(i));
        m_h_muRcv_dec->Fill(dec_vec.at(i));
      }
      for (std::vector<float>::size_type j = 0; j != thresh_vec.size(); ++j) {
        m_muRcv_thresh->push_back(thresh_vec.at(j));
        m_h_muRcv_thresh->Fill(thresh_vec.at(j));
      }
      for (std::vector<float>::size_type k = 0; k != ene_vec.size(); ++k) {
        m_muRcv_energy->push_back(ene_vec.at(k));
        m_h_muRcv_energy->Fill(ene_vec.at(k));
      }
      for (std::vector<float>::size_type l = 0; l != time_vec.size(); ++l) {
        m_muRcv_time->push_back(time_vec.at(l));
        m_h_muRcv_time->Fill(time_vec.at(l));
      }

      m_h_muRcvID->Fill(muRcvID);
    }
  }


  // Tile Container - TileTTL1Container
  // Raw Tile L1 Trigger Towers
  SG::ReadHandle<TileTTL1Container> p_ttl1MBTS_cont(m_inputMBTS_TTL1Key);
  if (p_ttl1MBTS_cont.isValid()) {
    // loop over TTL1 MBTS container
    TileTTL1Container::const_iterator ttl1MBTS_itr(p_ttl1MBTS_cont->begin());
    const TileTTL1Container::const_iterator ttl1MBTS_end(p_ttl1MBTS_cont->end());
    for ( ; ttl1MBTS_itr != ttl1MBTS_end; ++ttl1MBTS_itr ) {
      const Identifier ttl1MBTS_ID((*ttl1MBTS_itr)->identify());
      const std::vector<double> ttl1MBTS_digits((*ttl1MBTS_itr)->samples());

      const unsigned long long ttl1MBTS_ID_int = ttl1MBTS_ID.get_compact();
      m_ttl1MBTS_ID->push_back(ttl1MBTS_ID_int); // identifier
      m_ttl1MBTS_digits->push_back(ttl1MBTS_digits); // hardware sum of Tile channels; read out in N time slices

      for (std::vector<double>::size_type iy = 0; iy != ttl1MBTS_digits.size(); ++iy) {

        m_h_ttl1MBTS_digits->Fill(ttl1MBTS_digits.at(iy));
      }

      m_h_ttl1MBTS_ID->Fill(ttl1MBTS_ID_int);
    }
  }
  SG::ReadHandle<TileTTL1Container> p_ttl1Cont(m_inputTileTTL1Key);
  if (p_ttl1Cont.isValid()) {
    // loop over TTL1 container
    TileTTL1Container::const_iterator ttl1_itr(p_ttl1Cont->begin());
    const TileTTL1Container::const_iterator ttl1_end(p_ttl1Cont->end());
    for ( ; ttl1_itr != ttl1_end; ++ttl1_itr ) {
      const Identifier ttl1ID((*ttl1_itr)->identify());
      const std::vector<double> ttl1_digits((*ttl1_itr)->samples());

      const unsigned long long ttl1ID_int = ttl1ID.get_compact();
      m_ttl1_ID->push_back(ttl1ID_int);
      m_ttl1_digits->push_back(ttl1_digits);

      for (std::vector<double>::size_type jy = 0; jy != ttl1_digits.size(); ++jy) {
        m_h_ttl1_digits->Fill(ttl1_digits.at(jy));
      }

      m_h_ttl1_ID->Fill(ttl1ID_int);
    }
  }


  // Tile Container - TileL2
  // TileMuId and Et computed at TileCal ROD DSPs (use for L2 trigger)
  std::vector<unsigned int> val_vec;
  std::vector<float> eta_vec;
  std::vector<float> enemu0_vec;
  std::vector<float> enemu1_vec;
  std::vector<float> enemu2_vec;
  std::vector<unsigned int> qual_vec;
  std::vector<float> sumE_vec;

  SG::ReadHandle<TileL2Container> p_L2Cont(m_inputL2Key);
  if (p_L2Cont.isValid()) {
    // loop over L2 container
    TileL2Container::const_iterator L2_itr(p_L2Cont->begin());
    const TileL2Container::const_iterator L2_end(p_L2Cont->end());
    for ( ; L2_itr != L2_end; ++L2_itr ) {
      // drawer ID
      const int L2ID((*L2_itr)->identify());
      // packed muon info (32-bit words)
      for (unsigned int ii = 0; ii != (*L2_itr)->Ndata(); ii++) {
        val_vec.push_back((*L2_itr)->val(ii));
        m_h_L2val->Fill(val_vec.at(ii));
      }
      // muon info - energy deposited in TileCal layers, eta, quality flag
      for (unsigned int jj = 0; jj != (*L2_itr)->NMuons(); jj++) {
        eta_vec.push_back((*L2_itr)->eta(jj));
        enemu0_vec.push_back((*L2_itr)->enemu0(jj));
        enemu1_vec.push_back((*L2_itr)->enemu1(jj));
        enemu2_vec.push_back((*L2_itr)->enemu2(jj));
        qual_vec.push_back((*L2_itr)->qual(jj));

        m_h_L2eta->Fill(eta_vec.at(jj));
        m_h_L2energyA->Fill(enemu0_vec.at(jj));
        m_h_L2energyBC->Fill(enemu1_vec.at(jj));
        m_h_L2energyD->Fill(enemu2_vec.at(jj));
        m_h_L2qual->Fill(qual_vec.at(jj));
      }
      // drawer phi
      const float l2phi((*L2_itr)->phi(0));
      // vector sumE = [sumEt, sumEz, sumE] per TileCal superdrawer
      for (unsigned int kk = 0; kk != (*L2_itr)->NsumE(); kk++) {
        sumE_vec.push_back((*L2_itr)->sumE(kk));
        m_h_L2sumE->Fill(sumE_vec.at(kk));
      }

      m_L2ID->push_back(L2ID);
      m_L2val->push_back(val_vec);
      m_L2eta->push_back(eta_vec);
      m_L2energyA->push_back(enemu0_vec);
      m_L2energyBC->push_back(enemu1_vec);
      m_L2energyD->push_back(enemu2_vec);
      m_L2qual->push_back(qual_vec);
      m_L2phi->push_back(l2phi);
      m_L2sumE->push_back(sumE_vec);

      m_h_L2ID->Fill(L2ID);
      m_h_L2phi->Fill(l2phi);

      val_vec.clear();
      eta_vec.clear();
      enemu0_vec.clear();
      enemu1_vec.clear();
      enemu2_vec.clear();
      qual_vec.clear();
      sumE_vec.clear();
    }
  }


  // TileDigitsContainer - TileDigitsFlt
  SG::ReadHandle<TileDigitsContainer> p_digiCont(m_inputDigitsFltKey);
  if (p_digiCont.isValid()) {
    // loop over tile digits container
    TileDigitsContainer::const_iterator digiCont_itr(p_digiCont->begin());
    const TileDigitsContainer::const_iterator digiCont_end(p_digiCont->end());
    for ( ; digiCont_itr != digiCont_end; ++digiCont_itr ) {
      uint32_t fragSize((*digiCont_itr)->getFragSize());
      uint32_t fragBCID((*digiCont_itr)->getFragBCID());

      m_fragSize->push_back(fragSize);
      m_fragBCID->push_back(fragBCID);

      const TileRawDataCollection<TileDigits>* p_digiColl(*digiCont_itr);
      TileRawDataCollection<TileDigits>::const_iterator digi_itr(p_digiColl->begin());
      const TileRawDataCollection<TileDigits>::const_iterator digi_end(p_digiColl->end());
      for ( ; digi_itr != digi_end; ++digi_itr ) {
        const std::vector<double> digits((*digi_itr)->get_digits());
        m_digits->push_back(digits);

        for (std::vector<double>::size_type iz = 0; iz != digits.size(); ++iz) {
          m_h_digits->Fill(digits.at(iz));
        }
      }
    }
  }

  // TileDigitsContainer - MuRcvDigitsCnt
  SG::ReadHandle<TileDigitsContainer> p_mu_digiCont(m_inputDigitsMuRcvKey);
  if (p_mu_digiCont.isValid()) {
    // loop over tile digits container
    TileDigitsContainer::const_iterator muDigiCont_itr(p_mu_digiCont->begin());
    const TileDigitsContainer::const_iterator muDigiCont_end(p_mu_digiCont->end());
    for ( ; muDigiCont_itr != muDigiCont_end; ++muDigiCont_itr ) {
      const uint32_t muFragSize((*muDigiCont_itr)->getFragSize());
      const uint32_t muFragBCID((*muDigiCont_itr)->getFragBCID());

      m_muFragSize->push_back(muFragSize);
      m_muFragBCID->push_back(muFragBCID);

      const TileRawDataCollection<TileDigits>* p_mu_digiColl(*muDigiCont_itr);
      DataVector<TileDigits>::const_iterator muDigi_itr(p_mu_digiColl->begin());
      const DataVector<TileDigits>::const_iterator muDigi_end(p_mu_digiColl->end());
      for ( ; muDigi_itr != muDigi_end; ++muDigi_itr ) {
        const std::vector<double> muDigits((*muDigi_itr)->get_digits());
        m_muDigits->push_back(muDigits);

        for (std::vector<double>::size_type jz = 0; jz != muDigits.size(); ++jz) {
          m_h_muDigits->Fill(muDigits.at(jz));
        }
      }
    }
  }

  if (m_tree) {
    m_tree->Fill();
  }

  return StatusCode::SUCCESS;
}

StatusCode TileRDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
