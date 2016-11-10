/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALONE
int main(){return 0;}
#endif

#ifndef XAOD_STANDALONE

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
//#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"



#if 0

/////////////////////////////////////////////////////////////////////////////////////
/// remove these "==" tests, because the "==" operators for RoiDescriptors have 
/// been removed because they are misleading. Now RoIs have a range in both phi 
/// and z and have *independent* eta values at either end. RoIs with the same 
/// eta and phi, could *still* be completely different, because they have different 
/// z. Could add a test for equality of z also, but then, they could have the same 
/// central values, but different ranges, so could test for the same central values, 
/// and the same ranges, but then ...
/// In any case, on a pragmatic level, these tests are never used in the Trigger 
/// any more so they are not needed.
//////////////////////////////////////////////////////////////////

int testEqual_TrigRoiDescriptor() {
  MsgStream log(Athena::getMessageSvc(), "testEqual_TrigRoiDescriptor");  
  log << MSG::INFO << "testing identical RoiDescriptors" << endmsg;
  /// why these seemingly random RoI values?
  TrigRoiDescriptor a(0x4545ade3, 34, 2,  3.12, 3.12, 3.12, 0.17, 0.17, 0.17, 2.12, 2.12, 2.12);
  TrigRoiDescriptor b(0x4545ade3, 34, 2,  3.12, 3.12, 3.12, 0.17, 0.17, 0.17, 2.12, 2.12, 2.12);


  if ( !(a == b) ) {
    log << MSG::ERROR << "Identical RoIdescriptors found distinct! " << endmsg;
    return -1;
  }


  log << MSG::INFO << "testing distinct RoiDescriptors" << endmsg;
  TrigRoiDescriptor c(0x4545ade2, 34, 2, 3.12, 0.17, 2.12);
  TrigRoiDescriptor d(0x4545ade3, 34, 2, 3.13, 0.17, 2.12);
  TrigRoiDescriptor e(0x4545ade3, 34, 2, 3.12, -0.17, 2.12);
  if ( a == c ) {
    log << MSG::ERROR << "Different RoIdescriptors found same! " << endmsg;
    log << MSG::ERROR << a << endmsg;
    log << MSG::ERROR << c << endmsg;
    return -1;
  }

  if ( a == d ) {
    log << MSG::ERROR << "Different RoIdescriptors found same! " << endmsg;
    log << MSG::ERROR << a << endmsg;
    log << MSG::ERROR << d << endmsg;
    return -1;
  }

  if ( a == e ) {
    log << MSG::ERROR << "Different RoIdescriptors found same! " << endmsg;
    log << MSG::ERROR << a << endmsg;
    log << MSG::ERROR << e << endmsg;
    return -1;
  }
  return 0;
}

#endif

//////////////////////////////////////////////////////////////////
int testPrint_TrigRoiDescriptor() {
  TrigRoiDescriptor a;
  
  TrigRoiDescriptor b(0x4545ade3, 34, 2, 3.12, 0.17, 2.12);
  
  MsgStream log(Athena::getMessageSvc(), "testPrint_TrigRoiDescriptor");
  log << MSG::INFO << "MSG a " << a << endmsg;
  log << MSG::INFO << "MSG b " << b << endmsg;
  return 0;
}






//////////////////////////////////////////////////////////////////
int testEqual_TrigOperationalInfo() {
  TrigOperationalInfo a;
  a.set("va", 6.67);
  a.set("vb", 9.67);

  TrigOperationalInfo b;
  b.set("va", 6.67);
  b.set("vb", 9.67);


  TrigOperationalInfo c;
  c.set("va", 6.67);
  c.set("vb", 9.6);


  TrigOperationalInfo d; // size is different
  d.set("va", 6.67); 

  MsgStream log(Athena::getMessageSvc(), "testEqual_TrigOperationalInfo");
  if ( !(a == b)  ) {
    log << MSG::ERROR << "Identical TrigOperationalInfos found distinct! " << endmsg;
    return -1; 
  }
  

  if ( (a == c)  ) {
    log << MSG::ERROR << "Different TrigOperationalInfos found same! " << endmsg;
    return -1; 
  }


  if ( (a == d)  ) {
    log << MSG::ERROR << "Different TrigOperationalInfos found same! " << endmsg;
    return -1; 
  }

  return 0;
}



//////////////////////////////////////////////////////////////////
int testPrint_TrigOperationalInfo() {
  TrigOperationalInfo a;
  TrigOperationalInfo b;
  b.set("va", 6.67);
  b.set("vb", 9.67);

   MsgStream log(Athena::getMessageSvc(), "testPrint_TrigOperationalInfo");
   log << MSG::INFO << "MSG a " << a << endmsg;
   log << MSG::INFO << "MSG b " << b << endmsg;
  //  std::cout << "cout a" << a << std::endl;
  //  std::cout << "cout b" << b << std::endl;
  return 0; 
  
}



int main() {
  MsgStream log(Athena::getMessageSvc(), "Operators_test");

  // TrigRoiDescriptor

#if 0
  if ( testEqual_TrigRoiDescriptor() != 0 ) {
    log << MSG::ERROR << "TrigRoiDescriptor == operator is wrong" << endmsg;
    return -1;
  }
  log << MSG::INFO << "TrigRoiDescriptor == operator is correct " << endmsg;    
  if ( testPrint_TrigRoiDescriptor() != 0 ) {
    log << MSG::ERROR << "TrigRoiDescriptor << operators are wrong" << endmsg;
    return -1;
    
  }
  log << MSG::INFO << "TrigRoiDescriptor << operator is correct " << endmsg;    
#endif

  // TrigOperationalInfo

  if ( testEqual_TrigOperationalInfo() != 0 ) {
    log << MSG::ERROR << "TrigOperationalInfo == operator is wrong" << endmsg;
    return -1;
  }

  log << MSG::INFO << "TrigOperationalInfo == operator is correct " << endmsg;    
  if ( testPrint_TrigOperationalInfo() != 0 ) {
    log << MSG::ERROR << "TrigOperationalInfo << operators are wrong" << endmsg;
    return -1;
    
  }
  log << MSG::INFO << "TrigOperationalInfo << operator is correct " << endmsg;    



  return 0;
}
#endif
