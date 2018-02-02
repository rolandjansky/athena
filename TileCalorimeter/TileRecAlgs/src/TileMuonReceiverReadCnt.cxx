/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuonReceiverReadCnt.cxx 
//  Author   : Joao Gentil Saraiva <jmendes@cern.ch> 
//  Created  : April 2014
//
//  DESCRIPTION:
//    To read three containers of the TileContainer type used for Tile-d project
//
//          TileMuonReceiverContainer "TileMuRcvCnt"
//          TileRawChannelContainer   "MuRcvRawChannel" 
//          TileDigitsContainer       "MuRcvDigits"
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
//#include "TileEvent/TileContainer.h"
#include "TileRecAlgs/TileMuonReceiverReadCnt.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Gaudi includes
#include "GaudiKernel/ISvcLocator.h"

//C++ STL includes
#include <vector>
#include <algorithm>

using namespace std;

//==========================================================================
// TileMuonReceiverReadCnt's implementations
//==========================================================================

// Constructor
TileMuonReceiverReadCnt::TileMuonReceiverReadCnt(string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_tileHWID(0)
{
}

TileMuonReceiverReadCnt::~TileMuonReceiverReadCnt() {
}

// Alg standard interfacw function
StatusCode TileMuonReceiverReadCnt::initialize() {

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_digitsContainerKey.initialize() );
  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_muonReceiverContainerKey.initialize() );

  ATH_MSG_INFO( "TileMuonReceiverReadCnt initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverReadCnt::execute() {

  // step1: read the tile muon receiver container from TES
  //
  SG::ReadHandle<TileMuonReceiverContainer> muonReceiverContainer(m_muonReceiverContainerKey);
  if (muonReceiverContainer.isValid()) {

    ATH_MSG_INFO( "Reading the TileMuRcvContainer: " << m_muonReceiverContainerKey.key());

    // step2: check the number of entries that should be 128 per event
    //
    int nentries = muonReceiverContainer->size();
    ATH_MSG_INFO( "--- The number of container elements in " <<  m_muonReceiverContainerKey.key() << " is " << nentries );

    // step3: check container contents
    //

    for (const TileMuonReceiverObj* muonReceiverObj : *muonReceiverContainer) {

      int id = muonReceiverObj->GetID();
      ATH_MSG_INFO( "+-- Identifier of module: 0x" <<MSG::hex<< id <<MSG::dec<< " ros:" << (id>>8) << " module:"<< (id&0xff));

      // -- read thresholds
      const std::vector<float> & thresh = muonReceiverObj->GetThresholds();
      if (thresh.size()>=4)
        ATH_MSG_INFO( "--- thresholds' vector size : " << thresh.size() << " value: " << thresh[0] << " " << thresh[1] << " " << thresh[2] << " " << thresh[3] );
      else
        ATH_MSG_DEBUG( "--- thresholds' vector size : " << thresh.size() );

      // -- read energy
      const std::vector<float> & ene = muonReceiverObj->GetEne(); 
      if (ene.size()==2)
        ATH_MSG_INFO( "--- energy vector size : " << ene.size() << " value: " << ene[0] << " " << ene[1] );
      else
        ATH_MSG_DEBUG( "--- energy vector size : " << ene.size() ); 

      // -- read time
      const std::vector<float> & time = muonReceiverObj->GetTime(); 
      if (time.size()>=2)
        ATH_MSG_INFO( "--- time vector size : " << time.size() << " value: " << time[0] << " " << time[1] );
      else
        ATH_MSG_DEBUG( "--- time vector size : " << time.size() );

      // -- read decision
      const std::vector<bool> & decision = muonReceiverObj->GetDecision(); 
      if (decision.size()>=4)
        ATH_MSG_INFO( "--- decision vector size : " << decision.size() 
                      << " value: " << decision[0] << " " << decision[1] << " " << decision[2] << " " << decision[3] ); 
      else
        ATH_MSG_DEBUG( "--- decision vector size : " << decision.size() ); 
    }
  } else {
    ATH_MSG_INFO("Container " << m_muonReceiverContainerKey.key() << " not found") ;
  }

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey);
  if (rawChannelContainer.isValid()) {

    const float TMDB_AMPLITUDE_FACTOR = 1.0;

    ATH_MSG_INFO( "Reading the TileRawChannelContainer: " << m_rawChannelContainerKey.key());

    for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

      for(const TileRawChannel* rawChannel : *rawChannelCollection) {
        int frag_id = rawChannelCollection->identify();
        int ros=frag_id>>8;
        int drawer=(frag_id&0xFF);

        HWIdentifier hwid = rawChannel->adc_HWID() ;
        int channel       = m_tileHWID->channel(hwid) ;
        ATH_MSG_INFO("+-- hwid   : " << m_tileHWID->to_string(hwid,-1)) ;
        ATH_MSG_INFO("---- channel: " << channel 
                     << " ros: " << ros 
                     << " drawer: " << drawer
                     << " E[MeV]: " << lround(rawChannel->amplitude()*TMDB_AMPLITUDE_FACTOR));
      }
    }
    } else {
      ATH_MSG_INFO("Container " << m_rawChannelContainerKey.key() << " not found") ;
  }
  
  SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey);
  if (digitsContainer.isValid()) {

    ATH_MSG_INFO( "Reading the TileDigitsContainer: " << m_digitsContainerKey.key());

    for(const TileDigitsCollection* digitsCollection : *digitsContainer) {
      
      for (const TileDigits* tile_digits : *digitsCollection) {
        
        int frag_id = digitsCollection->identify();
        int ros=frag_id>>8;
        int drawer=(frag_id&0xFF);
        
        HWIdentifier hwid = tile_digits->adc_HWID() ;
        int channel       = m_tileHWID->channel(hwid) ;
        ATH_MSG_INFO("+-- hwid    : " << m_tileHWID->to_string(hwid,-1)) ;
        ATH_MSG_INFO("---- channel : " << channel << " ros: " << ros << " drawer: " << drawer);
        const std::vector<float> digits = tile_digits->samples();
        for (uint ismp=0;ismp<digits.size();ismp++) 
          ATH_MSG_INFO( "----- sample " << ismp << ": " << digits.at(ismp));
      }
    }
  } else {
    ATH_MSG_INFO("Container " << m_digitsContainerKey.key() << " not found") ;
  }
  
  // Execution completed.
  ATH_MSG_INFO( "TileMuonReceiverReadCnt execution completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverReadCnt::finalize() {

  ATH_MSG_INFO( "TileMuonReceiverReadCnt::finalize()" );

  return StatusCode::SUCCESS;
}
