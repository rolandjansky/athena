/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TestTools/initGaudi.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/HLTCheck.h"

using namespace HLT;
class ATestAlg : public HLT::FexAlgo {
public:
  ATestAlg(const std::string& name, ISvcLocator* pSvcLocator) 
    : HLT::FexAlgo(name, pSvcLocator){}

  virtual  HLT::ErrorCode hltExecute(const TriggerElement*, 
				     TriggerElement* ) {
    return HLT::OK;
    }
  
  virtual HLT::ErrorCode hltInitialize() { return HLT::OK; }
  virtual HLT::ErrorCode hltFinalize() { return HLT::OK; }

  HLT::ErrorCode returnEC(HLT::ErrorCode whatToReturn ) { return whatToReturn; }
  StatusCode returnSC(StatusCode whatToReturn ) { return whatToReturn; }



  HLT::ErrorCode testOK() {
    HLT_CHECK(returnEC(HLT::OK));
    return HLT::OK;
  }


  HLT::ErrorCode testAbortJob() {
    HLT_CHECK(returnEC(HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP)));
    return HLT::OK;
  }


  HLT::ErrorCode testAbortChain() {
    HLT_CHECK(returnEC(HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE)));
    return HLT::OK;
  }


  HLT::ErrorCode testComplain() {
    HLT_CHECK(returnEC(HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::MISSING_FEATURE)));
    return HLT::OK;
  }
  

  HLT::ErrorCode testSCSUCCESS() {    
    ATH_MSG_INFO("There should be no printout, all is supposed to go well");
    HLT_CHECK_SC( returnSC(StatusCode::SUCCESS), HLT::BAD_JOB_SETUP);
    return HLT::OK;
  }


  HLT::ErrorCode testSCFAILURE() {    
    HLT_CHECK_SC(returnSC(StatusCode::FAILURE), HLT::BAD_JOB_SETUP);
    return HLT::OK;
  }
  
};


int main() {
  using namespace std;

  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
    cerr << "ERROR This test can not be run" << endl;
    return 0;
  }
  assert(pSvcLoc);
    
  MsgStream log(Athena::getMessageSvc(), "HLTNavigation_test");
  ATestAlg alg("testAlgo", pSvcLoc);

  alg.testOK();
  alg.testAbortJob();
  alg.testAbortChain();
  alg.testComplain();
  alg.testSCSUCCESS();
  alg.testSCFAILURE();

  return 0;
}
