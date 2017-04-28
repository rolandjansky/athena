/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>

#undef NDEBUG
#include <cassert>


#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/ubsan_suppress.h"


#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/MonitoredScope.h"
#include "AthenaMonitoring/MonitoredScalar.h"





void resetHist(ITHistSvc* histSvc, const std::string& histName) {
  TH1* h(0);
  histSvc->getHist(histName, h);
  VALUE ( h ) NOT_EXPECTED ( nullptr );
  h->Reset();
}


double contentInBin1DHist(ITHistSvc* histSvc, const std::string& histName, int bin) {
  TH1* h(0);
  histSvc->getHist(histName, h);
  // this are in fact securing basic correctness of the tests
  VALUE ( h )   NOT_EXPECTED ( nullptr );  
  VALUE ( bin >= 1 ) EXPECTED ( true );
  VALUE ( bin <= h->GetXaxis()->GetNbins()+1 ) EXPECTED ( true );
  return h->GetBinContent(bin);
}

bool noToolBehaviourCorrect(ToolHandle<GenericMonitoringTool>& monTool) {
  using namespace Monitored;
  auto x = MonitoredScalar::declare("x", -99.0);
  auto monitorIt = MonitoredScope::declare(monTool, x);
  return true;
}


bool fillFromScalarWorked(ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc) {
  resetHist(histSvc, "/EXPERT/TestGroup/Eta");
  resetHist(histSvc, "/EXPERT/TestGroup/Phi");
  using namespace Monitored;
  {
    auto roiPhi = MonitoredScalar::declare("Phi", -99.0); //deducted double      
    auto roiEta = MonitoredScalar::declare<double>("Eta", -99); //explicit double
    auto monitorIt = MonitoredScope::declare(monTool, roiPhi, roiEta);
    roiPhi = 0.1;
    roiEta = -0.2;    
  }
  VALUE ( contentInBin1DHist(histSvc, "/EXPERT/TestGroup/Phi", 1) ) EXPECTED ( 0 );
  VALUE ( contentInBin1DHist(histSvc, "/EXPERT/TestGroup/Phi", 2) ) EXPECTED ( 1 );
  
  VALUE ( contentInBin1DHist(histSvc, "/EXPERT/TestGroup/Eta", 1) ) EXPECTED ( 1 );
  VALUE ( contentInBin1DHist(histSvc, "/EXPERT/TestGroup/Eta", 2) ) EXPECTED ( 0 );

  return true;
}

template<typename T>
class InvalidToolHandle : public ToolHandle<T> {
public:
  InvalidToolHandle() : ToolHandle<T>("") {}
  StatusCode retrieve( T*& ) const override {
    return StatusCode::FAILURE;
  }

};


int main() {
  //CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("GenericMon.txt",  pSvcLoc)) {
    std::cerr << "ERROR This test can not be run" << std::endl;
    return -1;
  }
  MsgStream log(Athena::getMessageSvc(), "GenericMonFilling_test");

  // we need to test what happens to the monitoring when tool is not valid
  InvalidToolHandle<GenericMonitoringTool> m_emptyMon;
  VALUE ( m_emptyMon.isValid() ) EXPECTED ( false ); // self test
  log << MSG::DEBUG << " mon tool validity " << m_emptyMon.isValid() << endmsg;

  ToolHandle<GenericMonitoringTool> m_validMon("GenericMonitoringTool/MonTool");
  if ( m_validMon.retrieve().isFailure() ) {
    log << MSG::ERROR << "Failed to acquire the MonTool tools via the ToolHandle" << endmsg;
    return -1;
  }

  ITHistSvc* histSvc;
  if( pSvcLoc->service("THistSvc", histSvc, true).isFailure()  ) {
    log << MSG::ERROR << "THistSvc not available " << endmsg;
    return -1;
  }
  assert( fillFromScalarWorked(m_validMon, histSvc) );
  assert( noToolBehaviourCorrect(m_emptyMon) );
  log << MSG::DEBUG << "All OK"  << endmsg;
  return 0;
}
