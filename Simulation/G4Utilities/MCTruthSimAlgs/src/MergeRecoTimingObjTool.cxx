/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeRecoTimingObjTool.h"

#include "AthenaKernel/errorcheck.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "RecEvent/RecoTimingObj.h"
#include "StoreGate/StoreGateSvc.h"

MergeRecoTimingObjTool::MergeRecoTimingObjTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_pMergeSvc("PileUpMergeSvc", name),
  m_recTimingObjKey("EVNTtoHITS_timings"),
  m_firstSubEvent(true)
{
  declareProperty("RecoTimingObjKey", m_recTimingObjKey);//=std::string("EVNTtoHITS_timings"));
}

StatusCode MergeRecoTimingObjTool::prepareEvent(unsigned int nInputEvents)
{
  ATH_MSG_DEBUG ( "Calling prepareEvent(): " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent=true;
  return StatusCode::SUCCESS;
}

StatusCode MergeRecoTimingObjTool::processBunchXing(int bunchXing,
                                                    SubEventIterator bSubEvents,
                                                    SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  if(m_firstSubEvent && bunchXing==0)
    {
      if (bSubEvents != eSubEvents)
        {
          StoreGateSvc& seStore(*bSubEvents->ptr()->evtStore());
          if(seStore.contains<RecoTimingObj>(m_recTimingObjKey.value()))
            {
              const RecoTimingObj *oldColl(0);
              CHECK(seStore.retrieve(oldColl, m_recTimingObjKey.value()));
              CHECK(processRecoTimingObj(oldColl));
            }
          else
            {
              ATH_MSG_DEBUG ( "processBunchXing: No RecoTimingObj found." );
            }
        }
      else
        {
          ATH_MSG_DEBUG ( "processBunchXing: Central Bunch Crossing Empty?!" );
        }
      m_firstSubEvent=false;
    }
  return StatusCode::SUCCESS;
}

StatusCode MergeRecoTimingObjTool::mergeEvent()
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

StatusCode MergeRecoTimingObjTool::processAllSubEvents()
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc)
    {
      CHECK(m_pMergeSvc.retrieve());
    }

  const RecoTimingObj *oldColl(NULL);
  if( !m_pMergeSvc->retrieveOriginal(m_recTimingObjKey.value(), oldColl).isSuccess() || !oldColl)
    {
      ATH_MSG_DEBUG ( "processAllSubEventss: Cannot find RecoTimingObj in input HITS file" );
      return StatusCode::SUCCESS;
    }
  return this->processRecoTimingObj(oldColl);
}


StatusCode MergeRecoTimingObjTool::processRecoTimingObj(const RecoTimingObj* inputObj)
{
  RecoTimingObj *outputRecoTimingObj = new RecoTimingObj(*inputObj); //CHECK copy constructor is OK!!
  CHECK(evtStore()->record(outputRecoTimingObj, m_recTimingObjKey.value()));
  ATH_MSG_DEBUG( "processRecoTimingObj: copied original event RecoTimingObj" );
  return StatusCode::SUCCESS;
}
