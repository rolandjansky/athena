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

#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"

// Tile includes
//#include "TileEvent/TileContainer.h"
#include "TileEvent/TileMuonReceiverContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileRecAlgs/TileMuonReceiverReadCnt.h"

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
  , m_TileMuRcvContainer("TileMuRcvCnt")
  , m_TileRawChannelContainer("MuRcvRawChCnt")
  , m_TileDigitsContainer("MuRcvDigitsCnt")
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

  ATH_MSG_INFO( "TileMuonReceiverReadCnt initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverReadCnt::execute() {

  // step1: read the tile muon receiver container from TES
  //
  const TileMuonReceiverContainer *container0 = evtStore()->tryConstRetrieve<TileMuonReceiverContainer>(m_TileMuRcvContainer);
  if (container0) {

    ATH_MSG_INFO( "Reading the TileMuRcvContainer: " << m_TileMuRcvContainer);

    // step2: check the number of entries that should be 128 per event
    //
    int nentries = container0->size();
    ATH_MSG_INFO( "--- The number of container elements in " <<  m_TileMuRcvContainer << " is " << nentries );

    // step3: check container contents
    //
    TileMuonReceiverContainer::const_iterator it = container0->begin();
    TileMuonReceiverContainer::const_iterator itLast = container0->end();

    for (; it != itLast; ++it) {

      const TileMuonReceiverObj * obj = (*it);

      int id = obj->GetID();
      ATH_MSG_INFO( "+-- Identifier of module: 0x" <<MSG::hex<< id <<MSG::dec<< " ros:" << (id>>8) << " module:"<< (id&0xff));

      // -- read thresholds
      const std::vector<float> & thresh = obj->GetThresholds();
      if (thresh.size()>=4)
        ATH_MSG_INFO( "--- thresholds' vector size : " << thresh.size() << " value: " << thresh[0] << " " << thresh[1] << " " << thresh[2] << " " << thresh[3] );
      else
        ATH_MSG_DEBUG( "--- thresholds' vector size : " << thresh.size() );

      // -- read energy
      const std::vector<float> & ene = obj->GetEne(); 
      if (ene.size()==2)
        ATH_MSG_INFO( "--- energy vector size : " << ene.size() << " value: " << ene[0] << " " << ene[1] );
      else
        ATH_MSG_DEBUG( "--- energy vector size : " << ene.size() ); 

      // -- read time
      const std::vector<float> & time = obj->GetTime(); 
      if (time.size()>=2)
        ATH_MSG_INFO( "--- time vector size : " << time.size() << " value: " << time[0] << " " << time[1] );
      else
        ATH_MSG_DEBUG( "--- time vector size : " << time.size() );

      // -- read decision
      const std::vector<bool> & decision = obj->GetDecision(); 
      if (decision.size()>=4)
        ATH_MSG_INFO( "--- decision vector size : " << decision.size() << " value: " << decision[0] << " " << decision[1] << " " << decision[2] << " " << decision[3] ); 
      else
        ATH_MSG_DEBUG( "--- decision vector size : " << decision.size() ); 
    }
  } else {
    ATH_MSG_INFO("Container " << m_TileMuRcvContainer << " not found") ;
  }

  const TileRawChannelContainer *container1 = evtStore()->tryConstRetrieve<TileRawChannelContainer>("TileRawChannelCnt");
  container1 = evtStore()->tryConstRetrieve<TileRawChannelContainer>(m_TileRawChannelContainer);
  if (container1) {

    const float TMDB_AMPLITUDE_FACTOR = 1.0;

    ATH_MSG_INFO( "Reading the TileRawChannelContainer: " << m_TileRawChannelContainer);

    TileRawChannelContainer::const_iterator collItr1  = container1->begin(); 
    TileRawChannelContainer::const_iterator lastColl1 = container1->end();

    for ( ; collItr1 != lastColl1; ++collItr1 ){
      TileRawChannelCollection::const_iterator chanItr1  = (*collItr1)->begin();
      TileRawChannelCollection::const_iterator lastChan1 = (*collItr1)->end();
      if (chanItr1==lastChan1) continue;
      int frag_id = (*collItr1)->identify();
      int ros=frag_id>>8;
      int drawer=(frag_id&0xFF);
      for ( ; chanItr1 != lastChan1; ++chanItr1 ){
        HWIdentifier hwid = (*chanItr1)->adc_HWID() ;
        int channel       = m_tileHWID->channel(hwid) ;
        ATH_MSG_INFO("+-- hwid   : " << m_tileHWID->to_string(hwid,-1)) ;
        //ATH_MSG_INFO("---- channel: " << channel << " ros: " << ros << " drawer: " << drawer<< " E[MeV]: " <<(*chanItr1)->amplitude()<<" t[ns]: "<<(*chanItr1)->time()<<" QF: "<<(*chanItr1)->quality());
        ATH_MSG_INFO("---- channel: " << channel << " ros: " << ros << " drawer: " << drawer<< " E[MeV]: " <<lround((*chanItr1)->amplitude()*TMDB_AMPLITUDE_FACTOR));
      }
    }
  } else {
    ATH_MSG_INFO("Container " << m_TileRawChannelContainer << " not found") ;
  }
  
  const TileDigitsContainer *container2 = evtStore()->tryConstRetrieve<TileDigitsContainer>("TileDigitsCnt");
  container2 = evtStore()->tryConstRetrieve<TileDigitsContainer>(m_TileDigitsContainer);
  if (container2) {

    TileDigitsContainer::const_iterator collItr2  = container2->begin(); 
    TileDigitsContainer::const_iterator lastColl2 = container2->end();

    ATH_MSG_INFO( "Reading the TileDigitsContainer: " << m_TileDigitsContainer);

    for ( ; collItr2 != lastColl2; ++collItr2 ){
      TileDigitsCollection::const_iterator chanItr2  = (*collItr2)->begin();
      TileDigitsCollection::const_iterator lastChan2 = (*collItr2)->end();
      if (chanItr2==lastChan2) continue;
      int frag_id = (*collItr2)->identify();
      int ros=frag_id>>8;
      int drawer=(frag_id&0xFF);
      for ( ; chanItr2 != lastChan2; ++chanItr2 ){
        HWIdentifier hwid = (*chanItr2)->adc_HWID() ;
        int channel       = m_tileHWID->channel(hwid) ;
        ATH_MSG_INFO("+-- hwid    : " << m_tileHWID->to_string(hwid,-1)) ;
        ATH_MSG_INFO("---- channel : " << channel << " ros: " << ros << " drawer: " << drawer);
        const std::vector<float> digits = (*chanItr2)->samples();
        for (uint ismp=0;ismp<digits.size();ismp++) ATH_MSG_INFO( "----- sample " << ismp << ": " << digits.at(ismp));
      }
    }
  } else {
    ATH_MSG_INFO("Container " << m_TileDigitsContainer << " not found") ;
  }

  // Execution completed.
  ATH_MSG_INFO( "TileMuonReceiverReadCnt execution completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverReadCnt::finalize() {

  ATH_MSG_INFO( "TileMuonReceiverReadCnt::finalize()" );

  return StatusCode::SUCCESS;
}
