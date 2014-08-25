/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/RPCStatusTestAlg.h"
#include "MuonCondSvc/MuonHierarchy.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"


RPCStatusTestAlg::RPCStatusTestAlg(
                         const std::string& name,
                         ISvcLocator* pSvcLocator ) :
                         AthAlgorithm( name, pSvcLocator ),
			 m_pSummarySvc("RPCCondSummarySvc", name){

}

RPCStatusTestAlg::~RPCStatusTestAlg()
{
  msg(MSG::INFO) << "Calling destructor" << endreq;
}

//Initialize
StatusCode
RPCStatusTestAlg::initialize(){
 
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling initialize" << endreq;
  sc = m_pSummarySvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endreq;
  }
  return sc;
}

//Execute
StatusCode
RPCStatusTestAlg::execute(){


  msg(MSG::INFO)<< "Calling execute" << endreq;
  msg(MSG::INFO)<<"Dummy call for the RPC STATUS"<<endreq;
  msg(MSG::INFO)<<"THE Panels efficiencies ARE: "<<endreq;
  m_pSummarySvc->EffPanelId();
  unsigned int size =m_pSummarySvc->EffPanelId().size();
  msg(MSG::VERBOSE) << "writing from Algo SERVICE \n" <<size <<endreq;
  for(unsigned int k=0;k<size;k++){
    Identifier panel = (m_pSummarySvc->EffPanelId()[k]);
    msg(MSG::VERBOSE) << "writing from Algo SERVICE Panel \n" << panel <<endreq;
  } 
  
 
 
  StatusCode sc(StatusCode::SUCCESS);

  return sc;
}


//Finalize
StatusCode
RPCStatusTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling finalize" << endreq;
  return sc;
}
