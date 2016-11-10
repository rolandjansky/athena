/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALON
int main(){return 0;}E
#endif

#ifndef XAOD_STANDALONE

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <algorithm>

#include "TrigSteeringEvent/HLTResult.h"





using namespace HLT;
int test_Truncation (HLTResult &res, int payload_size, unsigned expected_size, unsigned expected_chains_size, unsigned expected_nav_size, unsigned expected_extras_size ) {
  MsgStream log(Athena::getMessageSvc(), "Truncation_test");
  uint32_t* p1 = nullptr;

  
  int size = 0;
  log << MSG::INFO << "serialize with payload free " << payload_size  << endmsg;


  res.serialize(p1, size, payload_size);
  log << MSG::INFO << "serialized tot size " << size  << endmsg;

  if (size != int(expected_size) ) { 
    log << MSG::INFO << "serialized size distinct from expected, got: " << size << " expected: " << expected_size << endmsg;
    return -1;
  }
  
  HLTResult back;
  back.deserialize(p1, size);
  log << MSG::INFO << "Deserialized sizes chains: " <<  back.getChainResult().size() 
      << " navigation: " << back.getNavigationResult().size() 
      << " extras: " << back.getExtras().size() << endmsg;
  
  if ( back.getChainResult().size()  != expected_chains_size ) { 
    log << MSG::INFO << "deserialized chains size distinct from expected, got: " << back.getChainResult().size() << " expected: " << expected_chains_size << endmsg;
    return -1;
  }


  if ( back.getNavigationResult().size()  != expected_nav_size ) { 
    log << MSG::INFO << "deserialzied navigation size distinct from expected, got: " << back.getNavigationResult().size() << " expected: " << expected_nav_size << endmsg;
    return -1;
  }


  if ( back.getExtras().size()  != expected_extras_size ) { 
    log << MSG::INFO << "deserialzied extras size distinct from expected, got: " << back.getExtras().size() << " expected: " << expected_extras_size << endmsg;
    return -1;
  }
  delete p1;
  return 0;
}


int main() {
  MsgStream log(Athena::getMessageSvc(), "Truncation_test");
  // crate HLTResult
  HLTResult res;
  HLTResult back;

  std::vector<uint32_t> ch(800); // 800 chains
  res.getChainResult().insert(res.getChainResult().end(), ch.begin(), ch.end());
  std::vector<uint32_t> nav(4000);
  res.getNavigationResult().insert(res.getNavigationResult().end(), nav.begin(), nav.end());
  res.getNavigationResultCuts().push_back(1000);
  res.getNavigationResultCuts().push_back(2000);
  res.getNavigationResultCuts().push_back(3000);
  res.getNavigationResultCuts().push_back(3999);
  res.getNavigationResultCuts().push_back(4000);

  std::vector<uint32_t> ex(40);
  res.getExtras().insert(res.getExtras().end(), ex.begin(), ex.end());

  // try to serialzie it with very small payload

  log << MSG::INFO << "payload smaller than chains"  << endmsg;
  if ( test_Truncation(res, 600, 14, 0, 0, 0) != 0 ) {
    log << MSG::ERROR << "payload smaller than chains, this test failed"  << endmsg;
    return -1;
  }

  
  res.getNavigationResult() = nav;


  log << MSG::INFO << "payload bigger than chains"  << endmsg;
  if ( test_Truncation(res, 900, 816, 800, 0, 0) != 0 ) {
    log << MSG::ERROR << "payload bigger than chains, this test failed"  << endmsg;
    return -1;
  }


  res.getNavigationResult() = nav;

  log << MSG::INFO << "payload enough for first nav piece "  << endmsg;
  if ( test_Truncation(res, 1900, 1816, 800, 1000, 0) != 0 ) {
    log << MSG::ERROR << "payload enough for first nav piece, this test failed"  << endmsg;
    return -1;
  }

  res.getNavigationResult() = nav;

  log << MSG::INFO << "payload enough for almost all nav pieces "  << endmsg;
  if ( test_Truncation(res, 4816, 4816, 800, 4000, 0) != 0 ) {
    log << MSG::ERROR << "payload enough for almost all nav pieces, this test failed"  << endmsg;
    return -1;
  }

  res.getNavigationResult() = nav;

  log << MSG::INFO << "payload enough for  all nav pieces "  << endmsg;
  if ( test_Truncation(res, 4820, 4816, 800, 4000, 0) != 0 ) {
    log << MSG::ERROR << "payload enough for  all nav pieces, this test failed"  << endmsg;
    return -1;
  }

  res.getNavigationResult() = nav;

  log << MSG::INFO << "payload enough for  all  "  << endmsg;
  if ( test_Truncation(res, 5000, 4857, 800, 4000, 40) != 0 ) {
    log << MSG::ERROR << "payload enough for  all , this test failed"  << endmsg;
    return -1;
  }

  return 0;
}
#endif
