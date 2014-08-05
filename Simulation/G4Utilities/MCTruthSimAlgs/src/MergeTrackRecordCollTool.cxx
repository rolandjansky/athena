/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTrackRecordCollTool.h"

#include "AthenaKernel/errorcheck.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrackRecord/TrackRecordCollection.h"

MergeTrackRecordCollTool::MergeTrackRecordCollTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_pMergeSvc("PileUpMergeSvc", name),
  m_firstSubEvent(true)
{
  declareInterface<IPileUpTool>(this);
  declareProperty("TrackRecordCollKey", m_trRecCollKey=std::string("MuonEntryLayer"));
}

StatusCode MergeTrackRecordCollTool::prepareEvent(unsigned int nInputEvents) {
  ATH_MSG_DEBUG ( "Calling prepareEvent(): " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent=true;
  return StatusCode::SUCCESS;
}
StatusCode MergeTrackRecordCollTool::processBunchXing(int bunchXing,
                                                      PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                                                      PileUpEventInfo::SubEvent::const_iterator eSubEvents) {
  //We are only interested in the TrackRecordCollection for the original event
  if(m_firstSubEvent && bunchXing==0) {
    if (bSubEvents != eSubEvents) {
      StoreGateSvc& seStore(*bSubEvents->pSubEvtSG);
      const TrackRecordCollection* oldColl(0);
      if (seStore.retrieve(oldColl, m_trRecCollKey.value()).isSuccess()) {
        TrackRecordCollection* newColl = new TrackRecordCollection();
        TrackRecordCollection::const_iterator trcit = oldColl->begin();
        TrackRecordCollection::const_iterator trcend = oldColl->end();
        while(trcit != trcend) {
          newColl->push_back(new TrackRecord(**trcit) );
          ++trcit;
        }
        CHECK(evtStore()->record(newColl, m_trRecCollKey));
        ATH_MSG_DEBUG( "processBunchXing: copied original event TrackRecordCollection" );
        m_firstSubEvent=false;
      }
      else {
        ATH_MSG_ERROR ( "processBunchXing: TimedTruthList is empty" );
      }
    }
    else {
      ATH_MSG_ERROR ( "processBunchXing: Can not find TimedTruthList" );
    }
  }

  return StatusCode::SUCCESS;
}
StatusCode MergeTrackRecordCollTool::mergeEvent() {
  //Nothing to do here;
  return StatusCode::SUCCESS;
}

bool MergeTrackRecordCollTool::toProcess(int bunchXing) const {
  //We are only interested in the TrackRecordCollection for the original event
  //which should be in BC 0 !
  return (bunchXing==0);
}

StatusCode MergeTrackRecordCollTool::processAllSubEvents() {
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc) {
    if (!(m_pMergeSvc.retrieve()).isSuccess()) {
      ATH_MSG_FATAL ( "processAllSubEvents: Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }

  typedef PileUpMergeSvc::TimedList<TrackRecordCollection>::type TimedTruthList;
  TimedTruthList truthList;
  if ( (m_pMergeSvc->retrieveSubEvtsData(m_trRecCollKey.value(), truthList)).isSuccess() ) {
    if (truthList.begin() != truthList.end()) {
      //FIXME we are forced to do a deep copy
      const TrackRecordCollection &oldColl=*(truthList.begin())->second;
      TrackRecordCollection *newColl = new TrackRecordCollection();

      for (TrackRecordCollection::const_iterator trcit = oldColl.begin(); trcit != oldColl.end(); ++trcit) {
        newColl->push_back(new TrackRecord(**trcit) );
      }

      CHECK(evtStore()->record(newColl, m_trRecCollKey));
      ATH_MSG_DEBUG ( "processAllSubEvents: copied original event TrackRecordCollection" );
    } else ATH_MSG_ERROR ( "processAllSubEvents: TimedTruthList is empty" );
  } else ATH_MSG_ERROR ( "processAllSubEvents: Can not find TimedTruthList" );
  return StatusCode::SUCCESS;
}
