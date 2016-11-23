/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummaryTestAlg.cxx
 *
 * @brief Implementation file for the SCT_ConditionsSummaryTestAlg class 
 * in package SCT_ConditionsServices
 *
 * @author Shaun Roe
 **/

#include "SCT_SiliconConditionsTestAlg.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

//local includes
#include "SCT_SiliconConditionsSvc.h"

SCT_SiliconConditionsTestAlg::SCT_SiliconConditionsTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_siliconSvc("SCT_SiliconConditionsSvc", name)
{
  //nop
}

SCT_SiliconConditionsTestAlg::~SCT_SiliconConditionsTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endmsg;
}

//Initialize
StatusCode SCT_SiliconConditionsTestAlg::initialize()
{  
  msg(MSG::INFO) << "Calling initialize" << endmsg;
  if (m_siliconSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) <<"Could not retrieve the summary service"<<endmsg;
  }
  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_SiliconConditionsTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  
  msg(MSG::INFO) << "Calling execute" << endmsg;
  msg(MSG::INFO) << "Temperature  = " << m_siliconSvc->temperature(IdentifierHash(804)) << endmsg;
  msg(MSG::INFO) << "Depletion V  = " << m_siliconSvc->depletionVoltage(IdentifierHash(804)) << endmsg;
  msg(MSG::INFO) << "Bias Voltage = " << m_siliconSvc->biasVoltage(IdentifierHash(804)) << endmsg;  // was 216808130
  return StatusCode::SUCCESS;
}


//Finalize
StatusCode
SCT_SiliconConditionsTestAlg::finalize(){
  msg(MSG::INFO) << "Calling finalize" << endmsg;
  return StatusCode::SUCCESS;
}
