/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkRTT/DerivFwRtt.h"
#include "GaudiKernel/MsgStream.h"
/////////////////////////////////////////////////////////////////////////////
DerivFwRtt::DerivFwRtt(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator)
{
  // Properties go here
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode DerivFwRtt::initialize(){

    ATH_MSG_INFO ("initialize()");

    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode DerivFwRtt::execute() {

   ATH_MSG_INFO ("Your new package and algorithm are successfully executing");

    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode DerivFwRtt::finalize() {
    
    ATH_MSG_INFO ("finalize()");
    
    return StatusCode::SUCCESS;
}
