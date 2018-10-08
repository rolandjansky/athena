/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeHijingParsTool.h"

#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/make_unique.h"

MergeHijingParsTool::MergeHijingParsTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_pMergeSvc("PileUpMergeSvc", name),
  m_outputObject("Hijing_event_params"),
  m_firstSubEvent(true)
{
  declareProperty("HijingParamsKey", m_outputObject, "");
}

StatusCode MergeHijingParsTool::prepareEvent(unsigned int nInputEvents) {
  ATH_MSG_VERBOSE ( "Calling prepareEvent(): " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent=true;
  return StatusCode::SUCCESS;
}

StatusCode MergeHijingParsTool::processBunchXing(int,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  if(m_outputObject.isValid())
    {
      ATH_MSG_VERBOSE("Already written out a HijingEventParams for the current signal event.");
      return StatusCode::SUCCESS;
    }
  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++)
    {
      StoreGateSvc& seStore(*bSubEvents->ptr()->evtStore());
      if (seStore.contains<HijingEventParams>(m_outputObject.name()))
        {
          const HijingEventParams *hijing_pars(nullptr);
          ATH_CHECK(seStore.retrieve(hijing_pars, m_outputObject.name()));
          // create new container for overlayed event
          m_outputObject = CxxUtils::make_unique<HijingEventParams>(hijing_pars->get_np(),
                                                                    hijing_pars->get_nt(),
                                                                    hijing_pars->get_n0(),
                                                                    hijing_pars->get_n01(),
                                                                    hijing_pars->get_n10(),
                                                                    hijing_pars->get_n11(),
                                                                    hijing_pars->get_natt(),
                                                                    hijing_pars->get_jatt(),
                                                                    hijing_pars->get_b(),
                                                                    hijing_pars->get_bphi());
          // FIXME Why is there no copy constructor for this class?!
          // add in setting Psi angles manually.
          for(int n=1;n<7;++n)
            {
              m_outputObject->set_psi(n,hijing_pars->get_psi(n));
            }

          if(m_firstSubEvent)
            {
              ATH_MSG_DEBUG( "processBunchXing: copied original event HijingEventParams" );
            }
          else
            {
              ATH_MSG_DEBUG( "processBunchXing: copied background event HijingEventParams" );
            }
          return StatusCode::SUCCESS;
        }
      if(m_firstSubEvent)
        {
          ATH_MSG_VERBOSE("processBunchXing: No HijingEventParams found in the signal eventStore." );
          m_firstSubEvent=false;
        }
    }
  return StatusCode::SUCCESS;
}

StatusCode MergeHijingParsTool::mergeEvent()
{
  //Double check that something was found.
  if(!m_outputObject.isValid())
    {
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

StatusCode MergeHijingParsTool::processAllSubEvents() {
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc)
    {
      ATH_CHECK(m_pMergeSvc.retrieve());
    }
  typedef PileUpMergeSvc::TimedList<HijingEventParams>::type TimedHijingParamsList;
  TimedHijingParamsList HijingList;
  const HijingEventParams *hijing_pars(0);
  if (!(m_pMergeSvc->retrieveSubEvtsData(m_outputObject.name(), HijingList).isSuccess())
      || HijingList.size()==0) {
    ATH_MSG_INFO ( " Cannot find HijingEventParams from PileUp service " );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( " HijingEventParams found from PileUp service " );
  for (const auto& hijingParams: HijingList)
    {
      hijing_pars = hijingParams.second;
      // create new container for overlayed event
      m_outputObject = CxxUtils::make_unique<HijingEventParams>(hijing_pars->get_np(),
                                                                hijing_pars->get_nt(),
                                                                hijing_pars->get_n0(),
                                                                hijing_pars->get_n01(),
                                                                hijing_pars->get_n10(),
                                                                hijing_pars->get_n11(),
                                                                hijing_pars->get_natt(),
                                                                hijing_pars->get_jatt(),
                                                                hijing_pars->get_b(),
                                                                hijing_pars->get_bphi());
      // FIXME Why is there no copy constructor for this class?!
      // add in setting Psi angles manually.
      for(int n=1;n<7;++n)
        {
          m_outputObject->set_psi(n,hijing_pars->get_psi(n));
        }
      break;
    }
  //Double check that something was found.
  if(!m_outputObject.isValid())
    {
      ATH_MSG_ERROR( "processAllSubEvents: No HijingEventParams found in the signal or background eventStores." );
      return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
}
