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

// Tile includes

#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileEvent/TileRawChannelContainer.h"

#include "TileSimAlgs/TileMuonReceiverDecision.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileMuonReceiverObj.h"

TileMuonReceiverDecision::TileMuonReceiverDecision(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_MuRcvRawChContainer("MuRcvRawChCnt"),
    m_TileMuRcvContainer("TileMuRcvCnt"),
    m_tileID(0),
    m_tileHWID(0),
    m_cablingService(0)
{
  // declare properties...
  declareProperty( "MuonReceiverEneThreshCellD6Low"       , m_threshold_d6_lo   = 500. , "Setting the lowest trigger threshold for cell D6 in MeV (Def=500 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6andD5Low"  , m_threshold_d5d6_lo = 500. , "Setting the lowest trigger threshold for cell D5+D6 in MeV (Def=500 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6High"      , m_threshold_d6_hi   = 600. , "Setting the highest trigger threshold for cell D6 in MeV (Def=600 MeV)");
  declareProperty( "MuonReceiverEneThreshCellD6andD5High" , m_threshold_d5d6_hi = 600. , "Setting the highest trigger threshold for cell D5+D6 in MeV (Def=600 MeV)");
  declareProperty( "SelectionCutForMatchedFilterQf"       , m_selCutQf=0.              , "Selection cut for the quality factor of the matched filters");
  declareProperty( "TileMuonReceiverContainer"            , m_TileMuRcvContainer       , "Tile Calorimeter decision to TGC Sector Logic");
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

  // Create teh container to store the decision from the algorthm
  //  
  TileMuonReceiverContainer * decisionContainer = new TileMuonReceiverContainer();  

  TileMuonReceiverObj * TileMuRcvObj = new TileMuonReceiverObj(0,thresholds); // special object with thresholds
  decisionContainer->push_back(TileMuRcvObj);

  for ( ; collItr != lastColl; ++collItr )
    {
      TileRawChannelCollection::const_iterator chanItr  = (*collItr)->begin();
      TileRawChannelCollection::const_iterator lastChan = (*collItr)->end();

      if (chanItr==lastChan) continue;
      int frag_id = (*collItr)->identify();
      int ros=frag_id>>8;
      int drawer=(frag_id&0xFF);
      
      float energy_d6 = 0.0;
      float energy_d5d6 = 0.0;
      float time_d6 = 0.0;
      float time_d5d6 = 0.0;

      ATH_MSG_VERBOSE("A.00 " << tile2SL.size() );
      int j=2;

      for ( ; chanItr != lastChan; ++chanItr )
	{
	  // this is a temporary fix this is not being done in TilePulseForTileMuonReceiver

	  HWIdentifier hwid = (*chanItr)->adc_HWID() ;
	  int channel           = m_tileHWID->channel(hwid) ;
          
          if (msgLvl(MSG::VERBOSE)) {
            ATH_MSG_VERBOSE(" " << m_tileHWID->to_string(hwid,-1)) ;
            ATH_MSG_VERBOSE("channel " << channel << " ros " << ros << " drawer " << drawer);
	    ATH_MSG_VERBOSE("A.0"<< j <<" E[MeV] "<<(*chanItr)->amplitude()<<" t[ns] "<<(*chanItr)->time()<<" QF "<<(*chanItr)->quality());
          }
          
	  energy_d5d6 += (*chanItr)->amplitude();
          time_d5d6 += (*chanItr)->time()/4.;

	  if ( /*choose d6 cell*/ channel==37 || channel==38 ) {
              energy_d6 += (*chanItr)->amplitude();
              time_d6 += (*chanItr)->time()/2.;
          }
	  j++;
	}

      // A. for a clear reading first producing a bool format response for each thershold
      // d5+d6
      //
      bool pass_d5d6_hi = (energy_d5d6>m_threshold_d5d6_hi);
      bool pass_d5d6_lo = (energy_d5d6>m_threshold_d5d6_lo);

      // d6
      //
      bool pass_d6_hi = (energy_d6>m_threshold_d6_hi);
      bool pass_d6_lo = (energy_d6>m_threshold_d6_lo);


      // B. Now producing a vector with 4 bits each defining a unique tile decission
      //         HLHL:3210:[d5+d6][d6]
      //

      // output::bool decision entry 0 is the most significant digit in the 4 bits output
      //                       entry 3 is the least significant digit in the 4 bits output

      tile2SL[0] = pass_d5d6_hi;
      tile2SL[1] = (!pass_d5d6_hi && pass_d5d6_lo);
      tile2SL[2] = pass_d6_hi;
      tile2SL[3] = (!pass_d6_hi && pass_d6_lo);
      
      if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE("A.06 Summary : e(d5+d6)= " << energy_d5d6 << " e(d6)= " << energy_d6);
        ATH_MSG_VERBOSE("     Thresholds : " << m_threshold_d5d6_lo << " " << m_threshold_d5d6_hi << " " << m_threshold_d6_lo << " " << m_threshold_d6_hi );
        ATH_MSG_VERBOSE("A.07 Checking which tresholds have been passed: d56 high " << pass_d5d6_hi << " d56 low " << pass_d5d6_lo << " d6 high " << pass_d6_hi << " d6 low " << pass_d6_lo );
        ATH_MSG_VERBOSE("A.08 output to SL : " << tile2SL[0] << tile2SL[1] << tile2SL[2] << tile2SL[3] );
      }
      
      if (tile2SL[0] || tile2SL[1] || tile2SL[2] || tile2SL[3]) {

        int id_mod = ros*100+drawer;// 256*ros [3 eta>0 4 eta<0] + drawer [0-63 if <32 phi>0]

        ene[0]  = energy_d5d6;
        ene[1]  = energy_d6;
        time[0] = time_d5d6;
        time[1] = time_d6;

        TileMuonReceiverObj * TileMuRcvObj = new TileMuonReceiverObj(id_mod,ene,time,tile2SL);
        decisionContainer->push_back(TileMuRcvObj);

      } else {
        ATH_MSG_VERBOSE( " NULL trigger not include in container " );
      }
    }

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("== Print TileD decision container output to SL") ; 
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
