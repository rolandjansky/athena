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
  AthAlgorithm(name,pSvcLocator)
{
}

//----------------------------------------------------------------

BackgroundWordTest::~BackgroundWordTest()
{ 
}


//----------------------------------------------------------------

StatusCode BackgroundWordTest::initialize()
{
  ATH_MSG_INFO( "In initialize()" );
  return StatusCode::SUCCESS;  
}

//----------------------------------------------------------------

StatusCode BackgroundWordTest::execute()
{
   ///////////////////////// 
   // get the EventInfo
   /////////////////////////
   const EventInfo * eventInfo = 0;
   ATH_CHECK( evtStore()->retrieve( eventInfo ) );

   ////////////////////////////////////////
   // printout the final background word
   ///////////////////////////////////////
   ATH_MSG_DEBUG( "Summary of background word contents:");
   ATH_MSG_DEBUG( "MBTSTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffHalo));
   ATH_MSG_DEBUG( "MBTSTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffCol));
   ATH_MSG_DEBUG( "MBTSBeamVeto: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSBeamVeto));
   ATH_MSG_DEBUG( "LArECTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffHalo));
   ATH_MSG_DEBUG( "LArECTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffCol));
   ATH_MSG_DEBUG( "PixMultiplicityHuge: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::PixMultiplicityHuge));
   ATH_MSG_DEBUG( "PixSPNonEmpty: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::PixSPNonEmpty));
   ATH_MSG_DEBUG( "SCTMultiplicityHuge: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTMultiplicityHuge));
   ATH_MSG_DEBUG( "SCTSPNonEmpty: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTSPNonEmpty));
   ATH_MSG_DEBUG( "CSCTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffHalo));
   ATH_MSG_DEBUG( "CSCTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffCol));
   ATH_MSG_DEBUG( "BCMTimeDiffHalo: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffHalo));
   ATH_MSG_DEBUG( "BCMTimeDiffCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffCol));
   ATH_MSG_DEBUG( "BCMBeamVeto: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMBeamVeto));
   ATH_MSG_DEBUG( "MuonTimingCol: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCol));
   ATH_MSG_DEBUG( "MuonTimingCosmic: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCosmic));
   ATH_MSG_DEBUG( "LUCIDBeamVeto: "<<eventInfo->isEventFlagBitSet(EventInfo::Background,EventInfo::LUCIDBeamVeto));
   //

   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------

StatusCode BackgroundWordTest::finalize() {

  ATH_MSG_INFO( "In finalize()" );
  return StatusCode::SUCCESS;
}

