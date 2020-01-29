/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


///This test demonstrates how to write an algorithm
///And then use it with POOL::TEvent

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "xAODEventInfo/EventInfo.h"

class MyTestAlg : public AthAnalysisAlgorithm {
public:
  MyTestAlg( const std::string& name ) : AthAnalysisAlgorithm( name ) {
    declareProperty("MyProperty",m_value,"Example property");

  }

  virtual StatusCode initialize() {
    ATH_MSG_INFO(" initializing with MyProperty = " << m_value );
    return StatusCode::SUCCESS;
  }

  virtual StatusCode execute() {
    const xAOD::EventInfo* ei = 0;
    CHECK( evtStore()->retrieve( ei ) );
    ATH_MSG_INFO(" event num = " << ei->eventNumber());
    return StatusCode::SUCCESS;
  }

private:
  int m_value = 0;

};

//-------
// here's the example program

#include "POOLRootAccess/TEvent.h"

int main() {

  POOL::TEvent evt(POOL::TEvent::kClassAccess);
  evt.readFrom("$ASG_TEST_FILE_MC");


  MyTestAlg* alg = new MyTestAlg("MyAlg");
  alg->setProperty("MyProperty",4);

  alg->sysInitialize(); //calling sysInitialize means incident listening set up  

  for(int i=0;i<10;i++) {
    evt.getEntry(i);
    alg->execute();
  }

  return 0;

}
