/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataModel/DataVector.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
//#include  <iostream>
#include "TrigCombinedEvent/TrigComposite.h"
class ATest {
public:
  ATest(int arg){a = arg;}
  ~ATest() { std::cerr << " deleteing ATest: "<< a << std::endl; }
  int a;
};
CLASS_DEF(ATest, 6421, 1)
CLASS_DEF(DataVector<ATest>, 64210, 1)

int main() {
  MsgStream log(Athena::getMessageSvc(), "TrigComposite_test");
  using namespace std;
  TrigComposite atest("aobj");
  atest.addDetail("f1", 3.14f);
  atest.addDetail("f2", 3.18f);
  atest.addDetail("i1", 3);
  atest.addDetail("i2", 6);
  atest.addDetail<string>("s1", "worked");  
  atest.setDetail<string>("s1", "workedbetter");  
  atest.addObject("o1", TrigFeatureLink(6421, 0, 0)); 
  atest.addObject("o2", TrigFeatureLink(456578, 0, 0)); 
  if ( atest.getObjectsByType<ATest>().size() == 1 ) {
    log << MSG::INFO  << "By type Feature Links access works"  <<  endreq;
  } else {
    log << MSG::WARNING << "By type Feature access does not work" << endreq;
  }


  try {
    atest.setDetail("bla", 45);
    log << MSG::WARNING << "The exception is not thrown when setting inexisting detail" << endreq;
  } catch ( const std::invalid_argument& exc ) {
    log << MSG::INFO  << "correctly the exception is thrown"  << exc.what() << endreq;
  }


  TrigComposite btest("bobj");
  vector<string> intnames;
  intnames.push_back("x1");
  intnames.push_back("x2");

  btest.setFormat<int>(intnames, true);
  btest.lockFormat();
  if ( btest.isValid() )
    log << MSG::WARNING << "b object validity is true while shoudl be fale " << btest.isValid() << endreq;
  else
    log << MSG::INFO << "b object validity is rightly false " << btest.isValid() << endreq;
  
  try {
    btest.addDetail("ibla", 14);
    log << MSG::WARNING << "The exception is not thrown when adding the detail to the fixed format" << endreq;
  } catch ( const std::invalid_argument& exc ) {
    log << MSG::INFO  << "correctly the exception is thrown" << exc.what() << endreq;
  }


  


  
  log << atest << endreq;
  
  
  


}
