/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeHijingParsTool.h"

#include "AthenaKernel/errorcheck.h"
#include "GeneratorObjects/HijingEventParams.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"

MergeHijingParsTool::MergeHijingParsTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_pMergeSvc("PileUpMergeSvc", name),
  m_key("Hijing_event_params"),
  m_firstSubEvent(true)
{
  declareInterface<IPileUpTool>(this);
  declareProperty("HijingParamsKey", m_key=std::string("Hijing_event_params"));
}

StatusCode MergeHijingParsTool::prepareEvent(unsigned int nInputEvents) {
  ATH_MSG_DEBUG ( "Calling prepareEvent(): " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent=true;
  return StatusCode::SUCCESS;
}

StatusCode MergeHijingParsTool::processBunchXing(int bunchXing,
                                                SubEventIterator bSubEvents,
                                                SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  //We are only interested in the HijingEventParams for the original event
  if(m_firstSubEvent && bunchXing==0) {
    if (bSubEvents != eSubEvents) {
      StoreGateSvc& seStore(*bSubEvents->ptr()->evtStore());
      const HijingEventParams *hijing_pars(0);
      if (seStore.retrieve(hijing_pars, m_key.value()).isSuccess()) {

        // create new container for overlayed event
        HijingEventParams* newContainer = new HijingEventParams(hijing_pars->get_np(),
                                                                hijing_pars->get_nt(),
                                                                hijing_pars->get_n0(),
                                                                hijing_pars->get_n01(),
                                                                hijing_pars->get_n10(),
                                                                hijing_pars->get_n11(),
                                                                hijing_pars->get_natt(),
                                                                hijing_pars->get_jatt(),
                                                                hijing_pars->get_b(),
                                                                hijing_pars->get_bphi());

        // record new container in overlayed event
        if (!(evtStore()->record(newContainer,m_key.value()).isSuccess())) {
          ATH_MSG_ERROR ( " Cannot record new HijingEventParams in overlayed event " );
          return StatusCode::FAILURE;
        }

        ATH_MSG_DEBUG( "processBunchXing: copied original event HijingEventParams" );
        m_firstSubEvent=false;
      }
      else {
        ATH_MSG_ERROR ( "processBunchXing: No HijingEventParams found in the signal eventStore." );
      }
    }
    else {
      ATH_MSG_ERROR ( "processBunchXing: No events found." );
    }
  }

  return StatusCode::SUCCESS;
}
StatusCode MergeHijingParsTool::mergeEvent() {
  //Nothing to do here;
  return StatusCode::SUCCESS;
}

bool MergeHijingParsTool::toProcess(int bunchXing) const {
  //We are only interested in the HijingEventParams for the original event
  //which should be in BC 0 !
  return (bunchXing==0);
}

StatusCode MergeHijingParsTool::processAllSubEvents() {
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc) {
    if (!(m_pMergeSvc.retrieve()).isSuccess()) {
      ATH_MSG_FATAL ( "processAllSubEvents: Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }

  typedef PileUpMergeSvc::TimedList<HijingEventParams>::type TimedHijingParamsList;
  TimedHijingParamsList HijingList;
  const HijingEventParams *hijing_pars(0);
  if (!(m_pMergeSvc->retrieveSubEvtsData(m_key.value(), HijingList).isSuccess())
      || HijingList.size()==0) {
    ATH_MSG_INFO ( " Cannot find HijingEventParams from PileUp service " );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_INFO ( " HijingEventParams found from PileUp service " );
    TimedHijingParamsList::iterator firstHijing(HijingList.begin());
    // assume only 1 Hijing event in pileup
    hijing_pars = firstHijing->second;
  }

  // create new container for overlayed event
  HijingEventParams* newContainer = new HijingEventParams(hijing_pars->get_np(),
                                                          hijing_pars->get_nt(),
                                                          hijing_pars->get_n0(),
                                                          hijing_pars->get_n01(),
                                                          hijing_pars->get_n10(),
                                                          hijing_pars->get_n11(),
                                                          hijing_pars->get_natt(),
                                                          hijing_pars->get_jatt(),
                                                          hijing_pars->get_b(),
                                                          hijing_pars->get_bphi());

  // record new container in overlayed event
  if (!(evtStore()->record(newContainer,m_key.value()).isSuccess())) {
    ATH_MSG_ERROR ( " Cannot record new HijingEventParams in overlayed event " );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
