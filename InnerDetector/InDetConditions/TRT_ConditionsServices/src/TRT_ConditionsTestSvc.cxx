/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_ConditionsTestSvc.cxx
 *  @Service for example and test of TRT_ConditionsSummarySvc.
 *  Inherits from ITRT_ConditionsSvc and returns InDet::TRT_GOOD.
 *  @author Denver Whittington
 */

#include "TRT_ConditionsTestSvc.h"
#include "Identifier/Identifier.h"

//////////
/// Constructor
/////
TRT_ConditionsTestSvc::TRT_ConditionsTestSvc( const std::string& name,
					      ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator )
{
}

//////////
/// Destructor
/////
TRT_ConditionsTestSvc::~TRT_ConditionsTestSvc() {}

//////////
/// Initialize
/////
StatusCode TRT_ConditionsTestSvc::initialize() {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT_ConditionsTestSvc: Initialize." << endmsg;
  StatusCode sc(StatusCode::SUCCESS);

  return StatusCode::SUCCESS;
}

//////////
/// StatusFlag for TRT_ConditionsSummaryTool (by Identifier)
/////
InDet::TRT_CondFlag TRT_ConditionsTestSvc::condSummaryStatus( const Identifier& ) {
  return InDet::TRT_COND_GOOD;
}

//////////
/// Finalize
/////
StatusCode TRT_ConditionsTestSvc::finalize() {
  return StatusCode::SUCCESS;
}
