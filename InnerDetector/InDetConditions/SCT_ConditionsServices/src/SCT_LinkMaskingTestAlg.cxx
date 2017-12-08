/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_LinkMaskingTestAlg.cxx
 *
 * @brief 
 * 
 *
 * @author gwilliam@mail.cern.ch
 **/

#include "SCT_LinkMaskingTestAlg.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"

//local includes
#include "SCT_LinkMaskingSvc.h"

SCT_LinkMaskingTestAlg::SCT_LinkMaskingTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_linkMaskingSvc("SCT_LinkMaskingSvc", name)
{
  declareProperty("LinkMaskingSvc",                   m_linkMaskingSvc);
}

SCT_LinkMaskingTestAlg::~SCT_LinkMaskingTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endmsg;
}

//Initialize
StatusCode SCT_LinkMaskingTestAlg::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling initialize" << endmsg;
  
  // Retrieve link masking service
  if (m_linkMaskingSvc.retrieve().isFailure()) {
    msg(MSG::ERROR)<<"Could not retrieve the link masking service"<<endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_LinkMaskingTestAlg::execute(){

  msg(MSG::INFO) << "Wafer 167786496 is " << (m_linkMaskingSvc->isGood(Identifier(167786496)) ? "not masked":"masked") << endmsg;
  msg(MSG::INFO) << "Wafer 167787520 is " << (m_linkMaskingSvc->isGood(Identifier(167787520)) ? "not masked":"masked") << endmsg;

  return StatusCode::SUCCESS;
}


//Finalize
StatusCode SCT_LinkMaskingTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling finalize" << endmsg;
  return sc;
}
