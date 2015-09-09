/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/MsgStreamMember.h"

#include "TrigSteeringEvent/HLTResult.h"
#include "StreamTrigNavSlimming.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"
#include "TrigNavTools/TrigNavigationSlimmingHdlr.h"

#include <iostream>

HLT::TrigNavigationSlimmingHdlr::TrigNavigationSlimmingHdlr(HLT::HLTResult *result,
							    const INamedInterface* requester, 
							    HLT::TrigNavigationSlimmingTool *slimmer)
  : Athena::ISlimmingHdlr(),
    m_result(result), 
    m_requester(requester), 
    m_slimmer(slimmer) {
    
    // copy the nav data and cuts data so you can undo a slim 
    m_unslimmedNavData = m_result->getNavigationResult();
    m_unslimmedCuts = m_result->getNavigationResultCuts();
    m_name = requester->name() + "Hdlr";

}

HLT::TrigNavigationSlimmingHdlr::~TrigNavigationSlimmingHdlr() {
}

void HLT::TrigNavigationSlimmingHdlr::commit() {
  
  //  log << MSG::DEBUG << "TrigNavigationSlimmingHdlr::commit() wiht slimming tool " << m_slimmer << endreq;
  // do slimming and serialized to the m_result
  if( m_slimmer->doSlimming( m_result->getNavigationResult() ).isFailure() ) {
    Athena::MsgStreamMember mlog(Athena::Options::Eager, m_name);
    MsgStream log = mlog.get();    
    log << MSG::WARNING << "StreamTrigNavSlimming failed execute().  Unable to proceed... bailing out" << endreq;
    return;
  } 
  
  //log << MSG::DEBUG << "Performed actual slimming" << endreq;
}

void HLT::TrigNavigationSlimmingHdlr::rollback() {

  //  Athena::MsgStreamMember mlog(Athena::Options::Eager, m_name);
  //  MsgStream log = mlog.get();
  
  //  log << MSG::DEBUG << "TrigNavigationSlimmingHdlr::rollback()";

  // write the saved navigation data back into the hltresult
  std::vector<uint32_t>& navData = m_result->getNavigationResult();
  std::vector<unsigned int>& cuts = m_result->getNavigationResultCuts();

  navData.clear();
  navData.insert(navData.begin(), m_unslimmedNavData.begin(), m_unslimmedNavData.end());
  cuts.clear();
  cuts.insert(cuts.begin(), m_unslimmedCuts.begin(), m_unslimmedCuts.end());
  //  log << MSG::DEBUG << "Rolled back the slimming" << endreq;
}

void *HLT::TrigNavigationSlimmingHdlr::object() {

  return m_result;

}

const ::INamedInterface* HLT::TrigNavigationSlimmingHdlr::requester() {

  return m_requester;

}

std::type_info& HLT::TrigNavigationSlimmingHdlr::type_id() {

  const std::type_info& i(typeid(HLT::HLTResult));
  return const_cast<std::type_info&>(i);

}
