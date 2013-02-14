/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelToolTest.h"
#include "GaudiKernel/MsgStream.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/LArBadChanBitPacking.h"
#include "LArRecConditions/LArBadFebBitPacking.h"
#include "StoreGate/StoreGateSvc.h"

LArBadChannelToolTest::LArBadChannelToolTest(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_BadChanTool("LArBadChanTool"),
  m_detStore(0),
  m_Check1( 939685120),
  m_Check2( 944997120)
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty( "CheckChannel1", m_Check1);
  declareProperty( "CheckChannel2", m_Check2);

}

StatusCode LArBadChannelToolTest::initialize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  // Part 3: Retrieve the tools using the ToolHandles
  if ( m_BadChanTool.retrieve().isFailure() ) {
    log << MSG::FATAL  << m_BadChanTool.propertyName() << ": Failed to retrieve tool " 
	<< m_BadChanTool << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << m_BadChanTool.propertyName() << ": Retrieved tool " 
	<< m_BadChanTool.type() << endreq;
  }

  StatusCode sc = service("DetectorStore", m_detStore);
  if (!sc.isSuccess() || 0 == m_detStore)  {
    log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
  }
  sc = m_detStore->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArBadChannelToolTest::execute() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;

  LArBadChanBitPacking packing;

  log << MSG::INFO << "LArBadChannel status of channel " << m_Check1 << " is " 
      << m_BadChanTool->status(HWIdentifier(Identifier32(m_Check1))).packedData() << endreq;
  log << MSG::INFO << "LArBadChannel status of channel " << m_Check2 << " is " 
      << m_BadChanTool->status(HWIdentifier(Identifier32(m_Check2))).packedData() << endreq;

  std::vector<HWIdentifier>::const_iterator beg = m_onlineID->channel_begin();
  std::vector<HWIdentifier>::const_iterator end = m_onlineID->channel_end();
  for (;beg != end; ++beg) {
    LArBadChannel stat = m_BadChanTool->status( *beg);
    if ( !stat.good()) {
      log << MSG::INFO << "LArBadChannel status of channel " << *beg << " is " 
	  << packing.stringStatus(stat) << endreq;
    }
  }

  LArBadFebBitPacking febPacking;
  std::vector<HWIdentifier> mfebs = m_BadChanTool->missingFEBs();
  for (std::vector<HWIdentifier>::const_iterator ifeb=mfebs.begin(); ifeb!=mfebs.end(); ifeb++) {
    log << MSG::INFO << "FEB " << *ifeb << " is considered missing" << endreq;
  }

  typedef const std::vector< std::pair<HWIdentifier,LArBadFeb> >  BadFebVec;
  const BadFebVec& badFebs = m_BadChanTool->fullBadFebsState();
  for (BadFebVec::const_iterator i=badFebs.begin(); i!=badFebs.end(); ++i) {
    LArBadFeb stat = i->second;
    log << MSG::INFO << "LArBadFeb status of channel " << i->first << " is " 
	<< febPacking.stringStatus(stat) << endreq;
  }

  return StatusCode::SUCCESS;
}

