/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelTimingAlg.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArRecConditions/LArBadChanBitPacking.h"
#include "CaloIdentifier/LArEM_ID.h"

#include "GaudiKernel/Chrono.h"
#include <cstdlib>

// DEBUG only
#include <iostream>
#include <fstream>
#include <iterator>//for distance between iterators
using namespace std;

LArBadChannelTimingAlg::LArBadChannelTimingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator),
  m_BCKey("LArBadChannel"),
  m_cellID(0),
  m_reallyCheck(true)
{
  declareProperty("BadChanKey", m_BCKey);
  declareProperty("ReallyCheck", m_reallyCheck);

}

LArBadChannelTimingAlg::~LArBadChannelTimingAlg() {}


StatusCode LArBadChannelTimingAlg::initialize() {

  ATH_MSG_INFO ( "initialize()" );

  ATH_CHECK( m_BCKey.initialize() );

  ATH_CHECK( detStore()->retrieve(m_cellID,"CaloCell_ID") );
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_emID, "LArEM_ID") );
  return StatusCode::SUCCESS;
}

StatusCode LArBadChannelTimingAlg::execute() 
{
  // FIXME: execute the writing to the store only on first call
  static bool firstCall = true;
  if (firstCall) {
    fillChannelArray();
    firstCall = false;
  }
  timeOnlineAccess();
  timeOfflineAccess();
  timeFEBAccess();

  return StatusCode::SUCCESS;
}

void LArBadChannelTimingAlg::timeOnlineAccess() 
{
  unsigned long factor = ~(1ul<<31); 
  factor = factor / m_onlineID->channelHashMax() + 1; 
  ATH_MSG_INFO( "online random scaling factor " << factor );

  SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
  const LArBadChannelCont *bcCont {*readHandle};
  if(!bcCont) {
     ATH_MSG_WARNING( "Do not have Bad chan container " << m_BCKey.key() );
     return;
  }

  int nbad = 0;
  {
    Chrono chrono( chronoSvc() , "OnlineBadChan" );
    for (int i =0; i<testsPerEvent(); i++) {
      unsigned long rnd = random() / factor; // int arythmetic for speed to get a rnd channel hash
      HWIdentifier hid( m_hwarray.at(rnd));
      if ( m_reallyCheck) {
	if (!bcCont->status(hid).good()) nbad++;
      }
      else {
	if (rnd > 100000) nbad++;
      }
    }
  }
  ATH_MSG_INFO( "Number of randomly found online bad channels " << nbad );
}

void LArBadChannelTimingAlg::fillChannelArray() 
{
  m_hwarray.resize( m_onlineID->channelHashMax()+1);

  Chrono chrono( chronoSvc() , "fillChannelArray" );

  std::vector<HWIdentifier>::const_iterator hit = m_onlineID->channel_begin();
  for (; hit != m_onlineID->channel_end(); ++hit) {
    unsigned int hash =  m_onlineID->channel_Hash(*hit);
    m_hwarray.at(hash) = hit->get_identifier32().get_compact();
  }

  ATH_MSG_INFO( "m_hwarray.size() " << m_hwarray.size() << " capacity " << m_hwarray.capacity() );

}

StatusCode LArBadChannelTimingAlg::finalize()
{
  return StatusCode::SUCCESS;
}

void LArBadChannelTimingAlg::timeOfflineAccess() 
{
  // get reference to vector of all LArEM IDs (skip HEC & FCAL for now)
  const std::vector<Identifier>& allIDs = m_emID->channel_ids();

  unsigned long factor = ~(1ul<<31); 
  factor = factor / allIDs.size() + 1; 
  ATH_MSG_INFO( "connected offline IDs " << allIDs.size() << " offline random scaling factor " << factor );
  SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
  const LArBadChannelCont *bcCont {*readHandle};
  if(!bcCont) {
     ATH_MSG_WARNING( "Do not have Bad chan container " << m_BCKey.key() );
     return;
  }
  bcCont->offlineStatus( allIDs[0]); // first call takes much longer

  int nbad = 0;
  {
    Chrono chrono( chronoSvc() , "OfflineBadChan" );
    for (int i =0; i<testsPerEvent(); i++) {
      unsigned long rnd = random() / factor; // int arythmetic for speed to get a rnd channel hash
      Identifier id( allIDs.at(rnd));
      if ( m_reallyCheck) {
	if (!bcCont->offlineStatus(id).good()) nbad++;
      }
      else {
	if (rnd > 100000) nbad++;
      }
    }
  }
  ATH_MSG_INFO( "Number of randomly found offline bad channels " << nbad );

}

void LArBadChannelTimingAlg::timeFEBAccess() 
{
  std::vector<HWIdentifier> febVec;
  febVec.reserve( std::distance(m_onlineID->feb_begin(),m_onlineID->feb_end()));
  for (std::vector<HWIdentifier>::const_iterator feb = m_onlineID->feb_begin(); 
       feb != m_onlineID->feb_end(); ++feb) {
    febVec.push_back(*feb);
  }

  unsigned long factor = ~(1ul<<31); 
  factor = factor / febVec.size() + 1; 

  SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
  const LArBadChannelCont *bcCont {*readHandle};
  if(!bcCont) {
     ATH_MSG_WARNING( "Do not have Bad chan container " << m_BCKey.key() );
     return;
  }

  int nbad = 0;
  {
    Chrono chrono( chronoSvc() , "BadChanByFEB" );
    for (int i =0; i<testsPerEvent(); i++) {
      unsigned long rnd = random(); // all 31 bits random
      int rndFEB = rnd / factor; // int arythmetic for speed to get a rnd channel hash

      if ( m_reallyCheck) {
	if (!bcCont->status(HWIdentifier( febVec.at(rndFEB).get_compact() + (rnd & 127))).good()) nbad++;
      }
      else {
	if (rnd > 100000) nbad++;
      }
    }
  }
  ATH_MSG_INFO( "Number of randomly found bad channels by FEB " << nbad );
}
