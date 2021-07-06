/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdint.h>

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"

#include "TrigNavStructure/StandaloneNavigation.h"
#include "testutils.h"

using namespace std;
using namespace HLT;

bool sizesCheck(const std::vector<uint32_t>& serialized) {
  if (serialized[0] != 3 ) 
    REPORT_AND_STOP("Wrongly encoded version");

  if (serialized[1] != serialized.size() ) 
    REPORT_AND_STOP("Wrongly encoded size");

  if (serialized[2] != serialized.size() ) 
    REPORT_AND_STOP("Wrongly encoded TEs size");

  return true;  
}


bool empty() {
  BEGIN_TEST
  StandaloneNavigation tns;
  tns.reset();
  std::vector<uint32_t> serialized;
  bool status = tns.serialize(serialized);
  if ( status == false ) 
    REPORT_AND_STOP("Serialization of void TNS failed");
  PROGRESS;  
  if ( sizesCheck(serialized) == false ) 
    REPORT_AND_STOP("Serialized navigation sizes fail");

  if (serialized[3] != 0 ) 
    REPORT_AND_STOP("Wrongly encoded numner of TEs");
  PROGRESS;
  StandaloneNavigation post;
  status = post.deserialize(serialized);
  if ( status == false ) 
    REPORT_AND_STOP("deserialization of the empty navigation failed");
  MSG("OK", "Empty nav test passed");
  return 1;
}


bool singleTE() {
  BEGIN_TEST
  StandaloneNavigation tns;
  tns.reset();
  tns.getInitialNode();
  std::vector<uint32_t> serialized;
  bool status = tns.serialize(serialized);
  if ( status == false )
    REPORT_AND_STOP("Failed to serialzie navigation with one TE");
  PROGRESS;

  if ( sizesCheck(serialized) == false )
    REPORT_AND_STOP("Serialized navigation sizes fail");

  if ( serialized[3] != 1 ) 
    REPORT_AND_STOP("Number of TEs wrongly encoded");

  PROGRESS;
  StandaloneNavigation post;
  status = post.deserialize(serialized);
  if ( status == false )
    REPORT_AND_STOP("Failed to deserialzie navigation with one TE");

  MSG("OK", "One TE navigation serialization");
  return true;
}

bool realTEs() {
  BEGIN_TEST
  StandaloneNavigation tns;
  tns.reset();
  TriggerElement* roi = tns.addRoINode(tns.getInitialNode());
  TriggerElement* some = tns.addNode(roi, 878); // 878 is just some test ID  
  tns.addNode(roi, 878); 
  tns.addNode(some, 15); 

  std::vector<uint32_t> serialized;
  bool status = tns.serialize(serialized);
  if ( status == false )
    REPORT_AND_STOP("Failed to serialzie navigation with one TE");
  
  if ( sizesCheck(serialized) == false )
    REPORT_AND_STOP("Serialized navigation sizes fail");

  PROGRESS;
  if ( serialized[3] != 5 ) // initial one, RoI + the ones made explicitely with addNode
    REPORT_AND_STOP("Number of TEs wrongly encoded");
  std::vector<TriggerElement*> tes;
  tns.getAll(tes, false);
  const size_t nTEs = tes.size();

  PROGRESS;

  tns.reset();
  status = tns.deserialize(serialized);
  if ( status == false )
    REPORT_AND_STOP("Failed to deserialzie navigation with many TEs");

  if ( tns.countAllOfType(878) != 2 ) 
    REPORT_AND_STOP("After deserialization the TE with ID is missing");

  if ( tns.countAllOfType(15) != 1 ) 
    REPORT_AND_STOP("After deserialization the TE with ID is missing");
  PROGRESS;
  
  tes.clear();
  tns.getAll(tes, false);
  if ( tes.size() != nTEs ) 
    REPORT_AND_STOP("After deserialziation, wrong number of all TEs");
  


  MSG("OK", "One TE navigation serialization");
  return true;  
}


bool truncate() {
  BEGIN_TEST;
  StandaloneNavigation tns;
  tns.reset();
  TriggerElement* roi = tns.addRoINode(tns.getInitialNode());
  TriggerElement* some = tns.addNode(roi, 878); // 878 is just some test ID  
  tns.addNode(roi, 878); 
  tns.addNode(some, 15); 
  PROGRESS;

  std::vector<uint32_t> serialized;
  bool status = tns.serialize(serialized);
  if ( status == false )
    REPORT_AND_STOP("Failed to serialzie navigation with one TE");
  
  std::vector<uint32_t> truncated(serialized.begin(), serialized.end()-1);
  PROGRESS;

  tns.reset();
  PROGRESS;

  status = tns.deserialize(truncated);
  if ( status == true )
    REPORT_AND_STOP("deserialization worked on truncated payload");
  PROGRESS;
  
  return true;
}

int main() {

  // initialize Gaudi, SG
  ISvcLocator* pSvcLoc;
  Athena_test::initGaudi(pSvcLoc); 
  StoreGateSvc* pSG(nullptr);
  assert( pSvcLoc->service("StoreGateSvc", pSG, true).isSuccess() );

  // Create a context
  IProxyDict* xdict = &*pSG;
  xdict = pSG->hiveProxyDict();
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);

  if (not empty())
    ABORT("empty() test failed");

  if (not singleTE())
    ABORT("singleTE() test failed");

  if (not realTEs())
    ABORT("realTE() test failed");
    
  if ( not truncate() )
    ABORT("Truncation not noticed, bad as we read unknown memory");

  MSG("OK", "Test passed");
  return 0;
}
