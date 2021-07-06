/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeHijingParsTool.h"

MergeHijingParsTool::MergeHijingParsTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent) :
  PileUpToolBase(type, name, parent)
{
}

StatusCode MergeHijingParsTool::initialize() {
  ATH_CHECK(m_pMergeSvc.retrieve());
  ATH_CHECK(m_outputObjectKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode MergeHijingParsTool::prepareEvent(const EventContext& ctx, unsigned int nInputEvents) {
  ATH_MSG_VERBOSE ( "Calling prepareEvent()" );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent=true;
  m_outputObject = SG::makeHandle(m_outputObjectKey, ctx);
  return StatusCode::SUCCESS;
}

StatusCode MergeHijingParsTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  if(m_outputObject.isValid()) {
    ATH_MSG_VERBOSE("Already written out a HijingEventParams for the current signal event.");
    return StatusCode::SUCCESS;
  }
  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++) {
    const HijingEventParams *hijing_pars(nullptr);
    if (m_pMergeSvc->retrieveSingleSubEvtData(m_outputObject.name(), hijing_pars,
                                              bunchXing, iEvt).isSuccess()) {
      ATH_CHECK(m_outputObject.record(std::make_unique<HijingEventParams>(hijing_pars->get_np(),
                                                                          hijing_pars->get_nt(),
                                                                          hijing_pars->get_n0(),
                                                                          hijing_pars->get_n01(),
                                                                          hijing_pars->get_n10(),
                                                                          hijing_pars->get_n11(),
                                                                          hijing_pars->get_natt(),
                                                                          hijing_pars->get_jatt(),
                                                                          hijing_pars->get_b(),
                                                                          hijing_pars->get_bphi())));
      // FIXME Why is there no copy constructor for this class?!
      // add in setting Psi angles manually.
      for(int n=1;n<7;++n) {
        m_outputObject->set_psi(n,hijing_pars->get_psi(n));
      }

      if(m_firstSubEvent) {
        ATH_MSG_DEBUG( "processBunchXing: copied original event HijingEventParams" );
      }
      else {
        ATH_MSG_DEBUG( "processBunchXing: copied background event HijingEventParams" );
      }
      return StatusCode::SUCCESS;
    }

    if(m_firstSubEvent) {
      ATH_MSG_VERBOSE("processBunchXing: No HijingEventParams found in the signal eventStore." );
      m_firstSubEvent=false;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeHijingParsTool::mergeEvent(const EventContext& /*ctx*/)
{
  //Double check that something was found.
  if(!m_outputObject.isValid()) {
    ATH_MSG_ERROR ( "mergeEvent: No HijingEventParams found in the signal or background eventStores." );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool MergeHijingParsTool::toProcess(int bunchXing) const {
  //We are only interested in the HijingEventParams for the original event
  //which should be in BC 0 !
  return (bunchXing==0);
}

StatusCode MergeHijingParsTool::processAllSubEvents(const EventContext& ctx) {
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  m_outputObject = SG::makeHandle(m_outputObjectKey, ctx);
  typedef PileUpMergeSvc::TimedList<HijingEventParams>::type TimedHijingParamsList;
  TimedHijingParamsList HijingList;
  const HijingEventParams *hijing_pars(nullptr);
  if (!(m_pMergeSvc->retrieveSubEvtsData(m_outputObject.name(), HijingList).isSuccess())
      || HijingList.size()==0) {
    ATH_MSG_INFO ( " Cannot find HijingEventParams from PileUp service " );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( " HijingEventParams found from PileUp service " );
  for (const auto& hijingParams: HijingList) {
    hijing_pars = hijingParams.second;
    // create new container for overlayed event
    ATH_CHECK(m_outputObject.record(std::make_unique<HijingEventParams>(hijing_pars->get_np(),
                                                                        hijing_pars->get_nt(),
                                                                        hijing_pars->get_n0(),
                                                                        hijing_pars->get_n01(),
                                                                        hijing_pars->get_n10(),
                                                                        hijing_pars->get_n11(),
                                                                        hijing_pars->get_natt(),
                                                                        hijing_pars->get_jatt(),
                                                                        hijing_pars->get_b(),
                                                                        hijing_pars->get_bphi())));
    // FIXME Why is there no copy constructor for this class?!
    // add in setting Psi angles manually.
    for(int n=1;n<7;++n) {
      m_outputObject->set_psi(n,hijing_pars->get_psi(n));
    }
    break;
  }
  //Double check that something was found.
  if(!m_outputObject.isValid()) {
    ATH_MSG_ERROR( "processAllSubEvents: No HijingEventParams found in the signal or background eventStores." );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
