/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKAvailableHypo.h"

#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

//---------------------------------------------------------------------------------

TrigFTKAvailableHypo::TrigFTKAvailableHypo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator),
    m_acceptAll(0),
    m_ftk_in(false)
{


}

//---------------------------------------------------------------------------------

TrigFTKAvailableHypo::~TrigFTKAvailableHypo() {
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigFTKAvailableHypo::hltInitialize() {
  return HLT::OK;  
}

//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigFTKAvailableHypo::checkDetMask() {

  const xAOD::EventInfo* evinfo = nullptr;
  if (store()->retrieve(evinfo).isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve xAOD::EventInfo");
    return HLT::SG_ERROR;
  }
  else {
    ATH_MSG_DEBUG(evinfo->runNumber()
		  << " / " << evinfo->eventNumber()
		  << " / " << evinfo->lumiBlock()
		  << " / 0x" << std::hex << evinfo->detectorMask() << std::dec);

    uint64_t mask = evinfo->detectorMask();
    eformat::helper::DetectorMask decoder(mask);

    if (mask == 0) {
      ATH_MSG_DEBUG("DetMask=0");
      m_ftk_in=true;
    } else {
      m_ftk_in = decoder.is_set(eformat::TDAQ_FTK);
      ATH_MSG_DEBUG("FTK in DetMask " << m_ftk_in);
    }


  }
  return HLT::OK;
}

HLT::ErrorCode TrigFTKAvailableHypo::hltExecute(const HLT::TriggerElement* /* outputTE*/, bool& pass){

  pass = false;

  HLT::ErrorCode ec = checkDetMask();
  if (ec != HLT::OK) return ec;

  if (m_acceptAll || m_ftk_in)
    pass = true;

  return HLT::OK;
}

//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigFTKAvailableHypo::hltFinalize() {
  return HLT::OK;  
}

