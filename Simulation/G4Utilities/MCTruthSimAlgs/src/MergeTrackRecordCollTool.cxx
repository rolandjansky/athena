/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTrackRecordCollTool.h"

#include "AthenaKernel/errorcheck.h"

MergeTrackRecordCollTool::MergeTrackRecordCollTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent) :
  PileUpToolBase(type, name, parent)
{
}

StatusCode MergeTrackRecordCollTool::initialize()
{
  ATH_MSG_DEBUG( "initialize()" );
  ATH_CHECK( m_pMergeSvc.retrieve() );
  ATH_CHECK( m_outputKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode MergeTrackRecordCollTool::prepareEvent(const EventContext& /*ctx*/, unsigned int nInputEvents)
{
  ATH_MSG_DEBUG ( "Calling prepareEvent(): " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent=true;
  return StatusCode::SUCCESS;
}

StatusCode MergeTrackRecordCollTool::processBunchXing(int bunchXing,
                                                      SubEventIterator bSubEvents,
                                                      SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  //We are only interested in the TrackRecordCollection for the original event
  if(m_firstSubEvent && bunchXing==0) {
    if (bSubEvents != eSubEvents) {
      const TrackRecordCollection* oldColl(nullptr);
      if (m_pMergeSvc->retrieveSingleSubEvtData(m_trRecCollKey.value(), oldColl,
                                                bunchXing, bSubEvents).isSuccess()) {
        SG::WriteHandle<TrackRecordCollection> outputCollection(m_outputKey);
        ATH_CHECK(outputCollection.record(std::make_unique<TrackRecordCollection>()));
        if (!outputCollection.isValid()) {
          ATH_MSG_ERROR("Could not record output TrackRecordCollection " << outputCollection.name() << " to store " << outputCollection.store());
          return StatusCode::FAILURE;
        }

        for(const auto& trcit : *oldColl) {
          outputCollection->push_back( TrackRecord(trcit) );
        }
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

StatusCode MergeTrackRecordCollTool::mergeEvent(const EventContext& /*ctx*/)
{
  //Nothing to do here;
  return StatusCode::SUCCESS;
}

bool MergeTrackRecordCollTool::toProcess(int bunchXing) const
{
  //We are only interested in the TrackRecordCollection for the original event
  //which should be in BC 0 !
  return (bunchXing==0);
}

StatusCode MergeTrackRecordCollTool::processAllSubEvents(const EventContext& ctx)
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );

  typedef PileUpMergeSvc::TimedList<TrackRecordCollection>::type TimedTruthList;
  TimedTruthList truthList;
  if ( (m_pMergeSvc->retrieveSubEvtsData(m_trRecCollKey.value(), truthList)).isSuccess() ) {
    if (truthList.begin() != truthList.end()) {
      //FIXME we are forced to do a deep copy
      const TrackRecordCollection &oldColl=*(truthList.begin())->second;

      SG::WriteHandle<TrackRecordCollection> outputCollection(m_outputKey, ctx);
      ATH_CHECK(outputCollection.record(std::make_unique<TrackRecordCollection>()));
      if (!outputCollection.isValid()) {
        ATH_MSG_ERROR("Could not record output TrackRecordCollection " << outputCollection.name() << " to store " << outputCollection.store());
        return StatusCode::FAILURE;
      }

      for (const auto& trcit : oldColl) {
        outputCollection->push_back( TrackRecord(trcit) );
      }

      ATH_MSG_DEBUG ( "processAllSubEvents: copied original event TrackRecordCollection" );
    }
    else {
      ATH_MSG_ERROR ( "processAllSubEvents: TimedTruthList is empty" );
    }
  }
  else {
    ATH_MSG_ERROR ( "processAllSubEvents: Can not find TimedTruthList" );
  }
  return StatusCode::SUCCESS;
}
