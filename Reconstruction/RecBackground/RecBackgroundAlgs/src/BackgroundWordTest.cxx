/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecBackgroundAlgs/BackgroundWordTest.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"
#include <algorithm>

using xAOD::EventInfo;

//----------------------------------------------------------------

BackgroundWordTest::BackgroundWordTest(const std::string& name,
                                 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator)
{
}

//----------------------------------------------------------------

BackgroundWordTest::~BackgroundWordTest()
{ 
}


//----------------------------------------------------------------

StatusCode BackgroundWordTest::initialize()
{
  MsgStream logStr(messageService(),name());

  logStr << MSG::INFO << "In initialize()" << endreq;

  StatusCode sc;

  sc = service( "StoreGateSvc", m_storeGate);
  if( sc.isFailure() ) {
    logStr << MSG::FATAL << "Unable to locate the eventStore" << endreq;
    return sc;
  } 

  return StatusCode::SUCCESS;  
}

//----------------------------------------------------------------

StatusCode BackgroundWordTest::execute()
{
  StatusCode sc = StatusCode::SUCCESS;
   MsgStream msg(messageService(), name());   
   msg << MSG::DEBUG << "Executing " << name() << endreq;

   ///////////////////////// 
   // get the EventInfo
   /////////////////////////
   const EventInfo * eventInfo = 0;
   if (m_storeGate->retrieve( eventInfo ).isFailure() ) {
     msg << MSG::ERROR << "  Could not retrieve EventInfo object" << endreq;      
     return StatusCode::FAILURE;
   }
   ////////////////////////////////////////
   // printout the final background word
   ///////////////////////////////////////
   msg<<MSG::DEBUG << "Summary of background word contents:"<< endreq;
   msg<<MSG::DEBUG << "MBTSTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffHalo)<< endreq;
   msg<<MSG::DEBUG << "MBTSTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffCol)<< endreq;
   msg<<MSG::DEBUG << "MBTSBeamVeto: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSBeamVeto)<< endreq;
   msg<<MSG::DEBUG << "LArECTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffHalo)<< endreq;
   msg<<MSG::DEBUG << "LArECTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffCol)<< endreq;
   msg<<MSG::DEBUG << "PixMultiplicityHuge: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::PixMultiplicityHuge)<< endreq;
   msg<<MSG::DEBUG << "PixSPNonEmpty: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::PixSPNonEmpty)<< endreq;
   msg<<MSG::DEBUG << "SCTMultiplicityHuge: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTMultiplicityHuge)<< endreq;
   msg<<MSG::DEBUG << "SCTSPNonEmpty: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTSPNonEmpty)<< endreq;
   msg<<MSG::DEBUG << "CSCTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffHalo)<< endreq;
   msg<<MSG::DEBUG << "CSCTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffCol)<< endreq;
   msg<<MSG::DEBUG << "BCMTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffHalo)<< endreq;
   msg<<MSG::DEBUG << "BCMTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffCol)<< endreq;
   msg<<MSG::DEBUG << "BCMBeamVeto: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMBeamVeto)<< endreq;
   msg<<MSG::DEBUG << "MuonTimingCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCol)<< endreq;
   msg<<MSG::DEBUG << "MuonTimingCosmic: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCosmic)<< endreq;
   msg<<MSG::DEBUG << "LUCIDBeamVeto: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LUCIDBeamVeto)<< endreq;
   //

   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------

StatusCode BackgroundWordTest::finalize() {

  MsgStream logStr(messageService(),name());

  logStr << MSG::INFO << "In finalize()" << endreq;

  return StatusCode::SUCCESS;

}

