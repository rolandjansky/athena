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

SCT_TdaqEnabledTestAlg::SCT_TdaqEnabledTestAlg( 
                         const std::string& name, 
                         ISvcLocator* pSvcLocator ) : 
                         AthAlgorithm( name, pSvcLocator ), m_pTdaqEnabledSvc("SCT_TdaqEnabledSvc",name){
                           //nop
}

SCT_TdaqEnabledTestAlg::~SCT_TdaqEnabledTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endmsg;
}

//Initialize
StatusCode 
SCT_TdaqEnabledTestAlg::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO) << "Calling initialize" << endmsg;
  sc = m_pTdaqEnabledSvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the veto service"<<endmsg;
  }
  return sc;
}

//Execute
StatusCode 
SCT_TdaqEnabledTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO) << "Calling execute" << endmsg;
  msg(MSG::INFO) <<"Dummy call to module idHash 0: module is "<<endmsg;
  bool result=m_pTdaqEnabledSvc->isGood(IdentifierHash(0));
  msg(MSG::INFO) << (result?"good":"bad") << endmsg;
  msg(MSG::INFO) <<"Dummy call to module Identifier 1: module is "<<endmsg;
  result=m_pTdaqEnabledSvc->isGood(Identifier(1));
  msg(MSG::INFO) << (result?"good":"bad") << endmsg;
  msg(MSG::INFO) << "Using Identifier Hash method: with number 2137 "<<endmsg;
  result=m_pTdaqEnabledSvc->isGood(IdentifierHash(2137));
  msg(MSG::INFO) << (result?"good":"bad") << endmsg;
  msg(MSG::INFO) <<"Dummy call to module idHash 3: module is "<<endmsg;
  result=m_pTdaqEnabledSvc->isGood(IdentifierHash(3));
   msg(MSG::INFO) << (result?"good":"bad") << endmsg;
   unsigned int printNbad(10),printNgood(10);
   msg(MSG::INFO)<<"Printing the first "<<printNbad<<" bad modules, and the first "<<printNgood<<" good modules."<<endmsg;
   for (int i(0);i!=8176;++i){
     IdentifierHash idh(i);
     if ( printNbad and (not  m_pTdaqEnabledSvc->isGood(idh)) ){
        msg(MSG::INFO) <<i<<" is bad."<<endmsg;
        --printNbad;
      }
      if ( printNgood and m_pTdaqEnabledSvc->isGood(idh) ){
          msg(MSG::INFO) <<i<<" is good."<<endmsg;
          --printNgood;
        }
   }
  return sc;
}


//Finalize
StatusCode
SCT_TdaqEnabledTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "Calling finalize" << endmsg;
  return sc;
}
