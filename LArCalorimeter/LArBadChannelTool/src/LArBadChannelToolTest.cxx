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
  AthAlgorithm(name, pSvcLocator),
  m_BadChanTool("LArBadChanTool"),
  m_Check1( 939685120),
  m_Check2( 944997120)
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty( "CheckChannel1", m_Check1);
  declareProperty( "CheckChannel2", m_Check2);

}

StatusCode LArBadChannelToolTest::initialize() {

  ATH_MSG_INFO ( "initialize()" );

  ATH_CHECK ( m_BadChanTool.retrieve() );
  ATH_MSG_INFO ( m_BadChanTool.propertyName() << ": Retrieved tool " 
                 << m_BadChanTool.type() );
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );
  return StatusCode::SUCCESS;
}

StatusCode LArBadChannelToolTest::execute() {

  ATH_MSG_INFO ( "execute()" );

  LArBadChanBitPacking packing;

  ATH_MSG_INFO ( "LArBadChannel status of channel " << m_Check1 << " is " 
                 << m_BadChanTool->status(HWIdentifier(Identifier32(m_Check1))).packedData() );
  ATH_MSG_INFO ( "LArBadChannel status of channel " << m_Check2 << " is " 
                 << m_BadChanTool->status(HWIdentifier(Identifier32(m_Check2))).packedData() );

  std::vector<HWIdentifier>::const_iterator beg = m_onlineID->channel_begin();
  std::vector<HWIdentifier>::const_iterator end = m_onlineID->channel_end();
  for (;beg != end; ++beg) {
    LArBadChannel stat = m_BadChanTool->status( *beg);
    if ( !stat.good()) {
      ATH_MSG_INFO ( "LArBadChannel status of channel " << *beg << " is " 
                     << packing.stringStatus(stat) );
    }
  }

  LArBadFebBitPacking febPacking;
  std::vector<HWIdentifier> mfebs = m_BadChanTool->missingFEBs();
  for (std::vector<HWIdentifier>::const_iterator ifeb=mfebs.begin(); ifeb!=mfebs.end(); ifeb++) {
    ATH_MSG_INFO ( "FEB " << *ifeb << " is considered missing" );
  }

  typedef const std::vector< std::pair<HWIdentifier,LArBadFeb> >  BadFebVec;
  const BadFebVec& badFebs = m_BadChanTool->fullBadFebsState();
  for (BadFebVec::const_iterator i=badFebs.begin(); i!=badFebs.end(); ++i) {
    LArBadFeb stat = i->second;
    ATH_MSG_INFO ( "LArBadFeb status of channel " << i->first << " is " 
                   << febPacking.stringStatus(stat) );
  }

  return StatusCode::SUCCESS;
}

