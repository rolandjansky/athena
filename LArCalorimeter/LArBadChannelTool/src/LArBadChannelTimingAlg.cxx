/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelTimingAlg.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "LArBadChannelTool/LArBadChanTool.h"
#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArRecConditions/LArBadChanBitPacking.h"
#include "CaloIdentifier/LArEM_ID.h"

#include "GaudiKernel/Chrono.h"
#include "StoreGate/StoreGateSvc.h"
#include <cstdlib>

// DEBUG only
#include <iostream>
#include <fstream>
using namespace std;

LArBadChannelTimingAlg::LArBadChannelTimingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator),
  m_BadChanTool("LArBadChanTool"),
  m_cellID(0),
  m_reallyCheck(true)
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty("ReallyCheck", m_reallyCheck);

}

LArBadChannelTimingAlg::~LArBadChannelTimingAlg() {}


StatusCode LArBadChannelTimingAlg::initialize() {

  ATH_MSG_INFO ( "initialize()" );

  ATH_CHECK( m_BadChanTool.retrieve() );
  ATH_MSG_INFO ( m_BadChanTool.propertyName() << ": Retrieved tool " 
                 << m_BadChanTool.type() );

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
  long factor = ~(1<<31); 
  factor = factor / m_onlineID->channelHashMax() + 1; 
  cout << "online random scaling factor " << factor << endl;

  int nbad = 0;
  {
    Chrono chrono( chronoSvc() , "OnlineBadChan" );
    for (int i =0; i<testsPerEvent(); i++) {
      long rnd = random() / factor; // int arythmetic for speed to get a rnd channel hash
      HWIdentifier hid( m_hwarray.at(rnd));
      if ( m_reallyCheck) {
	if (!m_BadChanTool->status(hid).good()) nbad++;
      }
      else {
	if (rnd > 100000) nbad++;
      }
    }
  }
  cout << "Number of randomly found online bad channels " << nbad << endl;
}

void LArBadChannelTimingAlg::fillChannelArray() 
{
  // cout << "channelHashMax " << m_onlineID->channelHashMax() << endl;
  m_hwarray.resize( m_onlineID->channelHashMax()+1);

  Chrono chrono( chronoSvc() , "fillChannelArray" );

  std::vector<HWIdentifier>::const_iterator hit = m_onlineID->channel_begin();
  for (; hit != m_onlineID->channel_end(); ++hit) {
    unsigned int hash =  m_onlineID->channel_Hash(*hit);
    m_hwarray.at(hash) = hit->get_identifier32().get_compact();
  }

  cout << "m_hwarray.size() " << m_hwarray.size() 
       << " capacity " << m_hwarray.capacity() << endl;

}

StatusCode LArBadChannelTimingAlg::finalize()
{
  return StatusCode::SUCCESS;
}

void LArBadChannelTimingAlg::timeOfflineAccess() 
{
  // get reference to vector of all LArEM IDs (skip HEC & FCAL for now)
  const std::vector<Identifier>& allIDs = m_emID->channel_ids();

  long factor = ~(1<<31); 
  factor = factor / allIDs.size() + 1; 
  cout << "connected offline IDs " << allIDs.size() 
       << " offline random scaling factor " << factor << endl;
  m_BadChanTool->offlineStatus( allIDs[0]); // first call takes much longer

  int nbad = 0;
  {
    Chrono chrono( chronoSvc() , "OfflineBadChan" );
    for (int i =0; i<testsPerEvent(); i++) {
      long rnd = random() / factor; // int arythmetic for speed to get a rnd channel hash
      Identifier id( allIDs.at(rnd));
      if ( m_reallyCheck) {
	if (!m_BadChanTool->offlineStatus(id).good()) nbad++;
      }
      else {
	if (rnd > 100000) nbad++;
      }
    }
  }
  cout << "Number of randomly found offline bad channels " << nbad << endl;

}

void LArBadChannelTimingAlg::timeFEBAccess() 
{
  std::vector<HWIdentifier> febVec( m_onlineID->feb_end() - m_onlineID->feb_end());
  for (std::vector<HWIdentifier>::const_iterator feb = m_onlineID->feb_begin(); 
       feb != m_onlineID->feb_end(); ++feb) {
    febVec.push_back(*feb);
  }

  long factor = ~(1<<31); 
  factor = factor / febVec.size() + 1; 

  int nbad = 0;
  {
    Chrono chrono( chronoSvc() , "BadChanByFEB" );
    for (int i =0; i<testsPerEvent(); i++) {
      long rnd = random(); // all 31 bits random
      int rndFEB = rnd / factor; // int arythmetic for speed to get a rnd channel hash

      if ( m_reallyCheck) {
	if (!m_BadChanTool->status( febVec.at(rndFEB), rnd & 127).good()) nbad++;
      }
      else {
	if (rnd > 100000) nbad++;
      }
    }
  }
  cout << "Number of randomly found bad channels by FEB " << nbad << endl;
}
