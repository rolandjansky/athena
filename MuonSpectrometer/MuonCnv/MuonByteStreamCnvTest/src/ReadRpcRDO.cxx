/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*

  Test byte stream converters, write to and read from byte stream ...
  Ketevi A. Assamagan, CERN Jan-28-2003
  Adapted from H. Ma's test example!

*/

//#include <strstream>
#include <cassert>

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"

#include "MuonByteStreamCnvTest/ReadRpcRDO.h"

static const int maxPad           =   512;
static const int maxCMA           =  2048;
static const int maxFiredChannels =  4096;

/////////////////////////////////////////////////////////////////////////////

ReadRpcRDO::ReadRpcRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_ntuplePtr(0),
  m_activeStore("ActiveStoreSvc", name)
{
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteRpcNtuple", m_rpcNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadRpcRDO::initialize()
{
  ATH_CHECK( m_activeStore.retrieve() );

  if (!m_rpcNtuple) return StatusCode::SUCCESS;

  ATH_CHECK( accessNtuple() );

  //  Add the following items to the ntuple:
  //  --- numbers of pads, CMA and fired Channels
  //  --- pad, cma and fired channels data members

  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/npads",   m_nPads,          0, maxPad) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/ncmas",   m_nCMA,           0, maxCMA) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/fchan",   m_nFiredChannels, 0, maxFiredChannels) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/sector",  m_nPads,          m_sector) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/padId",   m_nPads,          m_padId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/status",  m_nPads,          m_status) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/ercode",  m_nPads,          m_errorCode) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/cmaId",   m_nCMA,           m_cmaId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/fel1Id",  m_nCMA,           m_fel1Id) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/febcId",  m_nCMA,           m_febcId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/crc",     m_nCMA,           m_crc) );  
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/bcId",    m_nFiredChannels, m_bcId) );  
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/time",    m_nFiredChannels, m_time) );  
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/ijk",     m_nFiredChannels, m_ijk) );  
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcrod/channel", m_nFiredChannels, m_channel) );  

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadRpcRDO::execute() {

  ATH_MSG_DEBUG( "in execute()"  );

  const RpcPadContainer* RpcRDO = nullptr; 
  ATH_CHECK( (*m_activeStore)->retrieve( RpcRDO, "RPCPAD" ) );

  ATH_MSG_DEBUG("****** RpcRDO->size() : " << RpcRDO->size() );

  if (!m_rpcNtuple) return StatusCode::SUCCESS;

 // Access by Collection 

 m_nPads = 0;
 m_nCMA = 0;
 m_nFiredChannels = 0;

 RpcPadContainer::const_iterator it1 = RpcRDO->begin(); 
 RpcPadContainer::const_iterator it2 = RpcRDO->end();

 for (; it1!=it2; ++it1){

   // loop over pads
   if ( (*it1)->size() > 0 ) {
     const RpcPad* pad = (*it1);
     assert (m_nPads < maxPad);
     m_sector[m_nPads]    = (long) pad->sector();
     m_padId[m_nPads]     = (long) pad->onlineId();
     m_status[m_nPads]    = (long) pad->status();
     m_errorCode[m_nPads] = (long) pad->errorCode();
     ATH_MSG_DEBUG( " Number of CMA in this pad " << (*it1)->size()  );

     // for each pad, loop over cma
     RpcPad::const_iterator it3 = (*it1)->begin(); 
     RpcPad::const_iterator it4 = (*it1)->end();
     for (; it3!=it4; ++it3) { 
       ATH_MSG_DEBUG( " Number of fired channels in this cma " << (*it3)->size()  );
       const RpcCoinMatrix * cma = (*it3);
       assert (m_nCMA < maxCMA);
       m_cmaId[m_nCMA]  = (long) cma->onlineId();
       m_fel1Id[m_nCMA] = (long) cma->fel1Id();
       m_febcId[m_nCMA] = (long) cma->febcId();
       m_crc[m_nCMA]    = (long) cma->crc();

       //for each cma for over fired channels
       RpcCoinMatrix::const_iterator it5 = (*it3)->begin(); 
       RpcCoinMatrix::const_iterator it6 = (*it3)->end();
       for (; it5!=it6; ++it5) { 
	 const RpcFiredChannel * fChannel = (*it5);
	 assert (m_nFiredChannels < maxFiredChannels);
	 m_bcId[m_nFiredChannels]    = (long) fChannel->bcid();
	 m_time[m_nFiredChannels]    = (long) fChannel->time();
	 m_ijk[m_nFiredChannels]     = (long) fChannel->ijk();
	 m_channel[m_nFiredChannels] = (long) fChannel->channel();
	 m_nFiredChannels++;
       }
       m_nCMA++;
     }
     m_nPads++;
   }
 }

 ATH_MSG_DEBUG( " done collecting histograms"  );
 ATH_MSG_DEBUG( "ReadRpcRDO::execute reports success" );
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadRpcRDO::finalize()
{
  ATH_MSG_INFO( "in finalize()"  );
  return StatusCode::SUCCESS;
}


StatusCode ReadRpcRDO::accessNtuple()
{
  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID ;

  //try to access it  
  NTuplePtr nt(ntupleSvc(), m_NtupleLocID );

  if ((int) nt)     {
     m_ntuplePtr=nt;
     ATH_MSG_INFO( "Ntuple " << m_NtupleLocID << " reaccessed! "  );
  } 
  else {
     ATH_MSG_FATAL( "Cannot reaccess " << m_NtupleLocID  );
     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


