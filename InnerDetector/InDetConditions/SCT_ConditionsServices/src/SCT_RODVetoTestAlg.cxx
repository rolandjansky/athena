/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTestAlg.cxx
 *
 * @brief Implementation file for the SCT_RODVetoTestAlg class 
 * in package SCT_ConditionsServices
 *
 * @author Daiki Hayakawa
 **/

#include "SCT_RODVetoTestAlg.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"

//local includes
#include "SCT_RODVetoSvc.h"

SCT_RODVetoTestAlg::SCT_RODVetoTestAlg( 
                         const std::string& name, 
                         ISvcLocator* pSvcLocator ) : 
                         AthAlgorithm( name, pSvcLocator ), m_pRODVetoSvc("SCT_RODVetoSvc",name){
                           //nop
}

SCT_RODVetoTestAlg::~SCT_RODVetoTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endreq;
}

//Initialize
StatusCode 
SCT_RODVetoTestAlg::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO) << "Calling initialize" << endreq;
  sc = m_pRODVetoSvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the veto service"<<endreq;
  }
  return sc;
}

//Execute
StatusCode 
SCT_RODVetoTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO) << "Calling execute" << endreq;
  msg(MSG::INFO) <<"Call to module in ROD 0x240100: Module is "<<endreq;
  bool result=m_pRODVetoSvc->isGood(0x807b00000000000);
  msg(MSG::INFO) << (result?"good":"bad") << endreq;
  IdentifierHash anyRandomHash(1000);
  result=m_pRODVetoSvc->isGood(anyRandomHash);
  msg(MSG::INFO) << (result?"good":"bad") << endreq;
 
  return sc;
}


//Finalize
StatusCode
SCT_RODVetoTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "Calling finalize" << endreq;
  return sc;
}
