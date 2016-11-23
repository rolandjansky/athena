/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleVetoTestAlg.cxx
 *
 * @brief Implementation file for the SCT_ModuleVetoTestAlg class 
 * in package SCT_ConditionsServices
 *
 * @author Shaun Roe
 **/

#include "SCT_ModuleVetoTestAlg.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"

//local includes
#include "SCT_ModuleVetoSvc.h"

SCT_ModuleVetoTestAlg::SCT_ModuleVetoTestAlg( 
                         const std::string& name, 
                         ISvcLocator* pSvcLocator ) : 
                         AthAlgorithm( name, pSvcLocator ), m_pModuleVetoSvc("SCT_ModuleVetoSvc",name){
                           //nop
}

SCT_ModuleVetoTestAlg::~SCT_ModuleVetoTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endmsg;
}

//Initialize
StatusCode 
SCT_ModuleVetoTestAlg::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO) << "Calling initialize" << endmsg;
  sc = m_pModuleVetoSvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the veto service"<<endmsg;
  }
  return sc;
}

//Execute
StatusCode 
SCT_ModuleVetoTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO) << "Calling execute" << endmsg;
  msg(MSG::INFO) <<"Dummy call to module id 0: module is "<<endmsg;
  bool result=m_pModuleVetoSvc->isGood(Identifier(0));
  msg(MSG::INFO) << (result?"good":"bad") << endmsg;
  msg(MSG::INFO) <<"Dummy call to module id 1: module is "<<endmsg;
  result=m_pModuleVetoSvc->isGood(Identifier(1));
  msg(MSG::INFO) << (result?"good":"bad") << endmsg;
  msg(MSG::INFO) << "Using Identifier Hash method: with number 2137 "<<endmsg;
  result=m_pModuleVetoSvc->isGood(IdentifierHash(2137));
  msg(MSG::INFO) << (result?"good":"bad") << endmsg;
  msg(MSG::INFO) <<"Dummy call to module id 3: module is "<<endmsg;
  result=m_pModuleVetoSvc->isGood(Identifier(3));
   msg(MSG::INFO) << (result?"good":"bad") << endmsg;
  return sc;
}


//Finalize
StatusCode
SCT_ModuleVetoTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "Calling finalize" << endmsg;
  return sc;
}
