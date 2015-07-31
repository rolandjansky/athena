/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuonReceiverDecision.cxx
// Author   : Joao Gentil Mendes Saraiva (jmendes@cern.ch)
// Created  : December 2013
//
// DESCRIPTION
// 
// Tile Muon Receiver decision
//
// Properties (JobOption Parameters):
//
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Calo includes

#include "CaloIdentifier/TileID.h"
#include "TileConditions/TileInfo.h"
// Tile includes

#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCondToolEmscale.h"

#include "TileSimAlgs/TileMuonReceiverDecision.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileMuonReceiverObj.h"

TileMuonReceiverDecision::TileMuonReceiverDecision(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_MuRcvRawChContainer("MuRcvRawChCnt"),
    m_TileMuRcvContainer("TileMuRcvCnt"),
    m_tileID(0),
    m_tileHWID(0),
    m_cablingService(0),
    m_tileInfo(0),
    m_tileToolEmscale("TileCondToolEmscale")
{
  // declare properties...
  declareProperty( "MuonReceiverEneThreshCellD6Low"       , m_threshold_d6_lo   = 500. , "Setting the lowest trigger threshold for cell D6 in MeV (Def=500 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6andD5Low"  , m_threshold_d5d6_lo = 500. , "Setting the lowest trigger threshold for cell D5+D6 in MeV (Def=500 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6High"      , m_threshold_d6_hi   = 600. , "Setting the highest trigger threshold for cell D6 in MeV (Def=600 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6andD5High" , m_threshold_d5d6_hi = 600. , "Setting the highest trigger threshold for cell D5+D6 in MeV (Def=600 MeV)");
  declareProperty( "SelectionCutForMatchedFilterQf"       , m_selCutQf=0.              , "Selection cut for the quality factor of the matched filters");
  declareProperty( "TileMuonReceiverContainer"            , m_TileMuRcvContainer       , "Tile Calorimeter decision to TGC Sector Logic");
  declareProperty( "TileCondToolEmscale"                  , m_tileToolEmscale          , "Service to calibrate all channels");
  declareProperty( "TileInfoName"                         , m_infoName = "TileInfo" );
}
 
TileMuonReceiverDecision::~TileMuonReceiverDecision()
{
}

StatusCode TileMuonReceiverDecision::initialize() {

  m_cablingService = TileCablingService::getInstance();

  if (m_cablingService->getCablingType() != TileCablingService::RUN2Cabling) {
     ATH_MSG_INFO("TileMuonReceiverDecision should not be used for RUN1 simulations");
     return StatusCode::SUCCESS;
  } else {
     ATH_MSG_INFO("Initializing TileMuonReceiverDecision");
  }

  //=== retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );
  CHECK(detStore()->retrieve(m_tileInfo, m_infoName));

  CHECK(m_tileToolEmscale.retrieve());

  ATH_MSG_INFO("TileMuonReceiverDecision initialization completed" );
  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverDecision::execute() {
  
  if (m_cablingService->getCablingType() != TileCablingService::RUN2Cabling) {
     ATH_MSG_VERBOSE( "ATT: RUN1 settings TileMuonReceiverDecision will end now" );
     return StatusCode::SUCCESS;
  } else {
     ATH_MSG_VERBOSE( "ATT: RUN2 settings TileMuonReceiverDecsionTool will run now" );
     ATH_MSG_DEBUG( "Executing TileMuonReceiverDecision" );
  }

  // Get the container with the matched filter reconstructed raw channels in MeV
  //
  const TileRawChannelContainer * MuonReceiverRawChannelContainer;
  CHECK( evtStore()->retrieve(MuonReceiverRawChannelContainer, m_MuRcvRawChContainer) ); 
  
  // The collection of raw channel containers are the drawers of extended barrel
  //
  TileRawChannelContainer::const_iterator collItr  = MuonReceiverRawChannelContainer->begin();
  TileRawChannelContainer::const_iterator lastColl = MuonReceiverRawChannelContainer->end();

  std::vector<bool>  tile2SL(4,false);
  std::vector<float> ene(2,0.0);
  std::vector<float> time(2,0.0);
  std::vector<float> thresholds;

  thresholds.push_back(m_threshold_d5d6_hi);
  thresholds.push_back(m_threshold_d5d6_lo);
  thresholds.push_back(m_threshold_d6_hi);
  thresholds.push_back(m_threshold_d6_lo);

  // Create the container to store the decision from the algorthm
  //  
  TileMuonReceiverContainer * decisionContainer = new TileMuonReceiverContainer();  

  TileMuonReceiverObj * TileMuRcvObj = new TileMuonReceiverObj(0,thresholds); // special object with thresholds
  decisionContainer->push_back(TileMuRcvObj);

  // conversion from TMDB channel number (0-3) to channel number in a drawer
  int Dchan[4]={17,16,37,38};

  for ( ; collItr != lastColl; ++collItr ) {

    TileRawChannelCollection::const_iterator chanItr  = (*collItr)->begin();
    TileRawChannelCollection::const_iterator lastChan = (*collItr)->end();

    if (chanItr==lastChan) continue;
    int frag_id   = (*collItr)->identify();
    int ros       = frag_id>>8;
    int drawer    = (frag_id&0xFF);
    //HWIdentifier drawer_id = m_tileHWID->drawer_id(frag_id);
    int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(frag_id);

    float energy_d6   = 0.0;
    float energy_d5d6 = 0.0;
    float time_d6     = 0.0;
    float time_d5d6   = 0.0;
    int jch6=0;
    int jch=0;

    ATH_MSG_VERBOSE("(E.00) The size of the tile2SL container was set to "<< tile2SL.size() <<" frag_id: "<< frag_id <<" ros: "<< ros <<" drawer: "<< drawer );

    for ( ; chanItr != lastChan; ++chanItr ) {
      // 
      // For TMDB channel numbers are being set differently (17,16,37,38)->(D5L,D5r,D6L,D6R)->(0,1,2,3)
      //

      HWIdentifier adc_id = (*chanItr)->adc_HWID() ;
      int TMDB_channel    = m_tileHWID->channel(adc_id) ;

      float ADC2MeV_factor = m_tileToolEmscale->channelCalib(drawerIdx, Dchan[TMDB_channel], TileID::LOWGAIN, 1.
                                                                      , TileRawChannelUnit::PicoCoulombs
                                                                      , TileRawChannelUnit::MegaElectronVolts) 
                           / m_tileInfo->MuRcvCalib(adc_id);

      float energy = (*chanItr)->amplitude()*ADC2MeV_factor;

      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "(E.0"<< jch+1 <<") hwid: "<< m_tileHWID->to_string(adc_id,-1) <<"  ch: "<< TMDB_channel <<" --> Tile HW channel: "<< Dchan[TMDB_channel] << endreq ;
        msg(MSG::VERBOSE) << "        E[ADC]: "<<(*chanItr)->amplitude()<<" E[MeV]: "<<energy<<" t[ns]: "<<(*chanItr)->time()<<" QF: "<<(*chanItr)->quality() << endreq;
      }

      energy_d5d6 += energy;
      time_d5d6   += (*chanItr)->time();
      ++jch;

      if ( TMDB_channel==2 || TMDB_channel==3 ) { /* choose d6 cell */
        energy_d6 += energy;
        time_d6   += (*chanItr)->time();
        ++jch6;
      }
    }
    if (jch>1) {
        time_d5d6 /= jch;
        if (jch6>1) time_d6 /= jch6;
    }
    
    // A. Above threshold (hi and lo) d5+d6 
    //
    bool pass_d5d6_hi = (energy_d5d6>m_threshold_d5d6_hi);
    bool pass_d5d6_lo = (energy_d5d6>m_threshold_d5d6_lo);

    //                    (hi and lo) d6
    //
    bool pass_d6_hi = (energy_d6>m_threshold_d6_hi);
    bool pass_d6_lo = (energy_d6>m_threshold_d6_lo);

    // B. Fill a vector with size 4 reproducing the 4-bit word with the structure HLHL:[d5+d6][d6]
    //    vector at 0 (d56hi) is the most significant digit in the 4 bits output
    //    vector at 3 (d6lo) is the least significant digit in the 4 bits output
    //
    tile2SL[0] = pass_d5d6_hi;
    tile2SL[1] = (!pass_d5d6_hi && pass_d5d6_lo);
    tile2SL[2] = pass_d6_hi;
    tile2SL[3] = (!pass_d6_hi && pass_d6_lo);
    
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "(E.05)   Summary: e(d5+d6)= " << energy_d5d6 << " e(d6)= " << energy_d6 << endreq;
      msg(MSG::VERBOSE) << "(E.06)   Thresholds: " << m_threshold_d5d6_lo << " " << m_threshold_d5d6_hi << " " << m_threshold_d6_lo << " " << m_threshold_d6_hi << endreq;
      msg(MSG::VERBOSE) << "(E.07)   Checking which tresholds have been passed: d56 high " << pass_d5d6_hi << " d56 low " << pass_d5d6_lo << " d6 high " << pass_d6_hi << " d6 low " << pass_d6_lo << endreq;
      msg(MSG::VERBOSE) << "(E.08)   Output to SL: " << tile2SL[0] << tile2SL[1] << tile2SL[2] << tile2SL[3] << endreq;
    }
    
    if (tile2SL[0] || tile2SL[1] || tile2SL[2] || tile2SL[3]) {

      int id_mod = frag_id; 

      ene[0]  = energy_d5d6;
      ene[1]  = energy_d6;
      time[0] = time_d5d6;
      time[1] = time_d6;

      TileMuonReceiverObj * TileMuRcvObj = new TileMuonReceiverObj(id_mod,ene,time,tile2SL);
      decisionContainer->push_back(TileMuRcvObj);

    } else {
      ATH_MSG_VERBOSE( "== NULL trigger not include in container " );
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "== Print TileD decision container output to SL" << endreq ; 
    decisionContainer->print();
  }
  CHECK(evtStore()->record( decisionContainer, m_TileMuRcvContainer, false));

  ATH_MSG_DEBUG("TileMuonReceiverDecision execution completed" );
  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverDecision::finalize() {
  ATH_MSG_DEBUG( "Finalizing TileMuonReceiverDecision");
  
  ATH_MSG_INFO( "TileMuonReceiverDecision finalized successfully");
  return StatusCode::SUCCESS;
}
