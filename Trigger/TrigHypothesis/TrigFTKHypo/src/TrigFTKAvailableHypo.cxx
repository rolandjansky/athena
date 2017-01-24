/*
  Copyright (C) 1995-2017 CERN for the benefit of the ATLAS collaboration
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "TrigFTKAvailableHypo.h"

#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/IssueSeverity.h"

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

  const xAOD::EventInfo* evinfo = 0;
  if (store()->retrieve(evinfo).isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve xAOD::EventInfo")
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
    }

    if( msgLvl() <= MSG::INFO ){
      m_log << MSG::INFO << "sct_barrel_a_side is " << (m_sct_barrel_a_side==true? "present" : "OFF! ") << endreq;
    }
  }
  return HLT::OK;
}

HLT::ErrorCode TrigFTKAvailableHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

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

