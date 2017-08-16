/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledTestAlg.cxx
 *
 * @brief Implementation file for the SCT_TdaqEnabledTestAlg class 
 * in package SCT_ConditionsServices
 *
 * @author Shaun Roe
 **/

#include "SCT_TdaqEnabledTestAlg.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"

//local includes
#include "SCT_TdaqEnabledSvc.h"

SCT_TdaqEnabledTestAlg::SCT_TdaqEnabledTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_pTdaqEnabledSvc{"SCT_TdaqEnabledSvc", name} {
  //nop
}

SCT_TdaqEnabledTestAlg::~SCT_TdaqEnabledTestAlg()
{ 
  ATH_MSG_INFO("Calling destructor");
}

//Initialize
StatusCode
SCT_TdaqEnabledTestAlg::initialize() {
  StatusCode sc{StatusCode::SUCCESS, true};
  ATH_MSG_INFO("Calling initialize");
  sc = m_pTdaqEnabledSvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Could not retrieve the veto service");
  }
  return sc;
}

//Execute
StatusCode 
SCT_TdaqEnabledTestAlg::execute() {
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  StatusCode sc{StatusCode::SUCCESS, true};
  ATH_MSG_INFO("Calling execute");
  ATH_MSG_INFO("Dummy call to module idHash 0: module is ");
  bool result{m_pTdaqEnabledSvc->isGood(IdentifierHash{0})};
  ATH_MSG_INFO((result ? "good" : "bad"));
  ATH_MSG_INFO("Dummy call to module Identifier 1: module is ");
  result=m_pTdaqEnabledSvc->isGood(Identifier{1});
  ATH_MSG_INFO((result ? "good" : "bad"));
  ATH_MSG_INFO("Using Identifier Hash method: with number 2137 ");
  result=m_pTdaqEnabledSvc->isGood(IdentifierHash{2137});
  ATH_MSG_INFO((result ? "good" : "bad"));
  ATH_MSG_INFO("Dummy call to module idHash 3: module is ");
  result=m_pTdaqEnabledSvc->isGood(IdentifierHash{3});
  ATH_MSG_INFO((result ? "good" : "bad"));
  unsigned int printNbad{10}, printNgood{10};
  ATH_MSG_INFO("Printing the first " << printNbad << " bad modules, and the first " << printNgood << " good modules.");
  for (unsigned int i{0}; i<8176; ++i) {
    IdentifierHash idh{i};
     if (printNbad and (not m_pTdaqEnabledSvc->isGood(idh))) {
       ATH_MSG_INFO(i << " is bad.");
       --printNbad;
     }
     if (printNgood and m_pTdaqEnabledSvc->isGood(idh)) {
       ATH_MSG_INFO(i << " is good.");
       --printNgood;
     }
   }
  return sc;
}

//Finalize
StatusCode
SCT_TdaqEnabledTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
