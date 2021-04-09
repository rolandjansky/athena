/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeRecoTimingObjTool.h"

#include "AthenaKernel/errorcheck.h"
#include "RecEvent/RecoTimingObj.h"

MergeRecoTimingObjTool::MergeRecoTimingObjTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent) :
  PileUpToolBase(type, name, parent)
{
}

StatusCode MergeRecoTimingObjTool::initialize()
{
  ATH_CHECK(m_pMergeSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MergeRecoTimingObjTool::prepareEvent(const EventContext& /*ctx*/, unsigned int nInputEvents)
{
  ATH_MSG_DEBUG ( "Calling prepareEvent()" );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent=true;
  return StatusCode::SUCCESS;
}

StatusCode MergeRecoTimingObjTool::processBunchXing(int bunchXing,
                                                    SubEventIterator bSubEvents,
                                                    SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  if(m_firstSubEvent && bunchXing==0) {
    if (bSubEvents != eSubEvents) {
      const RecoTimingObj *oldColl(nullptr);
      if (m_pMergeSvc->retrieveSingleSubEvtData(m_recTimingObjInputKey.value(), oldColl,
                                                bunchXing, bSubEvents).isSuccess()) {
        ATH_CHECK(processRecoTimingObj(oldColl));
      }
      else {
        ATH_MSG_DEBUG ( "processBunchXing: No RecoTimingObj found." );
      }
    }
    else {
      ATH_MSG_DEBUG ( "processBunchXing: Central Bunch Crossing Empty?!" );
    }
    m_firstSubEvent=false;
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeRecoTimingObjTool::mergeEvent(const EventContext& /*ctx*/)
{
  //Nothing to do here;
  return StatusCode::SUCCESS;
}

bool MergeRecoTimingObjTool::toProcess(int bunchXing) const
{
  //We are only interested in the RecoTimingObj for the original event
  //which should be in BC 0 !
  return (bunchXing==0);
}

StatusCode MergeRecoTimingObjTool::processAllSubEvents(const EventContext& /*ctx*/)
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  const RecoTimingObj *oldColl(nullptr);
  if( !m_pMergeSvc->retrieveOriginal(m_recTimingObjInputKey.value(), oldColl).isSuccess() || !oldColl) {
    ATH_MSG_DEBUG ( "processAllSubEventss: Cannot find RecoTimingObj in input HITS file" );
    return StatusCode::SUCCESS;
  }
  return this->processRecoTimingObj(oldColl);
}


StatusCode MergeRecoTimingObjTool::processRecoTimingObj(const RecoTimingObj* inputObj)
{
  RecoTimingObj *outputRecoTimingObj = new RecoTimingObj(*inputObj); //CHECK copy constructor is OK!!
  ATH_CHECK(evtStore()->record(outputRecoTimingObj, m_recTimingObjOutputKey.value()));
  ATH_MSG_DEBUG( "processRecoTimingObj: copied original event RecoTimingObj" );
  return StatusCode::SUCCESS;
}
