/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include "TrigNavigation/TriggerElement.h"


#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"


#include "TestUtils.h"

using namespace std;
using namespace HLT;

StatusCode unit0(const TriggerElement::ObjectIndex& orig, unsigned expSerializedSize, MsgStream& log ) {
  BEGIN_TEST("Unit0 test");
  vector<uint32_t> serial;
  vector<uint32_t>::const_iterator sIt;
  TriggerElement::ObjectIndex deserial;
  orig.serialize(serial);

  if (serial.size() != expSerializedSize) 
    REPORT_AND_STOP( "orig ObjIdx serialized has size !=  " << expSerializedSize << " "  << serial.size() );

  for ( sIt = serial.begin(); sIt != serial.end(); ++sIt ) {
    log << MSG::INFO << "serialized word: 0x" << MSG::hex <<  *sIt << MSG::dec << endmsg;
  }
  sIt = serial.begin();
  deserial.deserialize(sIt);
  
  if ( deserial.isSameOrWithin(&orig) && orig.isSameOrWithin(&deserial)) {
    log << MSG::INFO << "serialziation works well for orig     " << orig << endmsg;
    log << MSG::INFO << "original     " << orig << endmsg;
    log << MSG::INFO << "deserialized " << deserial << endmsg;
    
  } else REPORT_AND_STOP( "orig ObjIdx serialization failed " );

  END_TEST;
  
}


int main () {  

  /*
   ISvcLocator* pSvcLoc;
   if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
     cerr << "ERROR This test can not be run" << endl;
     return 0;
   }
   assert(pSvcLoc);
  */

  MsgStream log(Athena::getMessageSvc(), "TriggerElement_test");
  msglog = &log;

  log << MSG::INFO << "small " << endmsg;
  // test ObjectIndex:
  TriggerElement::ObjectIndex small1(12, 4, 5);
  if ( unit0(small1, 1, log).isFailure() ) 
    ABORT("");

  TriggerElement::ObjectIndex small2(15, 4, 7);
  if ( unit0(small2, 1, log).isFailure() )
    ABORT("");


  REPORT_AND_CONTINUE("big ");  
  TriggerElement::ObjectIndex big1(345, 60000, 60001);
  if ( unit0(big1, 2, log).isFailure() )
    ABORT("");

  TriggerElement::ObjectIndex big2(345, 60000, 62345);
  if ( unit0(big2, 2, log).isFailure() )
    ABORT("");


  log << MSG::INFO << "huge "  << endmsg;
  try {
  TriggerElement::ObjectIndex huge1(12, 65535, 65536);
  if ( unit0(huge1, 3, log).isFailure() )
    ABORT("");
  } catch (std::exception& ex) {
    ABORT("Execption from the test " <<  ex.what() );
  }
  TriggerElement::ObjectIndex huge2(12, 820000, 820001);
  if ( unit0(huge2, 3, log).isFailure() )
    ABORT("");
  
  REPORT_AND_CONTINUE("SUCCESS");
  return 0;
}
