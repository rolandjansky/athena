/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DigitizationTests/PileUpEventInfoTest.h"

#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include "EventInfo/EventType.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"
PileUpEventInfoTest::PileUpEventInfoTest(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_pileupInfo("McEventInfo")
{
  declareProperty("EventInfoName", m_pileupInfo);
}


StatusCode PileUpEventInfoTest::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}


StatusCode PileUpEventInfoTest::execute()
{
  const PileUpEventInfo* pevt = 0;
  if ( evtStore()->retrieve(pevt, m_pileupInfo.value()).isFailure() ) {
    ATH_MSG_ERROR ( "** Could not get pileup event info" );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG ( "Pileup Info Retrieved Successfully as 'PileUpEventInfo' Object " );
    //+++ Get sub-event info object
    EventID *eventid = pevt->event_ID();
    ATH_MSG_DEBUG ( "Main Event Info: " );
    ATH_MSG_INFO ( "RunNumber = " << eventid->run_number() );
    ATH_MSG_INFO ( "Event Number = " << eventid->event_number() );
    ATH_MSG_INFO ( "LumiBlock = " << eventid->lumi_block() );
    ATH_MSG_INFO ( "TimeStamp = " << eventid->time_stamp() );
    ATH_MSG_INFO ( "BCID = " << eventid->bunch_crossing_id() );
    ATH_MSG_INFO ( "mu = " << pevt->actualInteractionsPerCrossing() );
    ATH_MSG_INFO ( "<mu> = " << pevt->averageInteractionsPerCrossing() );

    ATH_MSG_INFO ( "Sub Event Infos: " );
    PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
    if (it == end) ATH_MSG_INFO ( "None found" );
    for (; it != end; ++it) {
      const EventInfo* sevt = (*it).pSubEvt;
      if (sevt!=NULL) {
        ATH_MSG_INFO ( "Sub Event Info:" );
        ATH_MSG_INFO ( "  Time         : " << (*it).time()                             << endreq
                       << "  Index        : " << (*it).index()                            << endreq
                       << "  Provenance   : " << (*it).type()                        << endreq // This is the provenance stuff: signal, minbias, cavern, etc
                        << "  Run Number   : " << sevt->event_ID()->run_number()           << endreq
                        << "  Event Number : " << sevt->event_ID()->event_number()         << endreq
                        << "  ns Offset    : " << sevt->event_ID()->time_stamp_ns_offset() << endreq
                        << "  Lumi Block   : " << sevt->event_ID()->lumi_block()           << endreq
                        << "  BCID         : " << sevt->event_ID()->bunch_crossing_id()    << endreq
                        << "  User Type    : " << sevt->event_type()->user_type()          );
        // if ((*it).time() == 0) {
        //   NInTimeEvents++;
        //   if ((*it).index() > 0) NInTimePileup++;
        // }
      }
      else ATH_MSG_INFO ( "Subevent is null ptr " );
    }
  }
  return StatusCode::SUCCESS;
}

