/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ProtosLHEF_i/protos2lhef.h"
#include "ProtosLHEF_i/lhefTrf.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <stdio.h>

protos2lhef::protos2lhef(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

StatusCode protos2lhef::initialize() {

  int retval = 0;

  // Remove any previous events.lhe file if exists
  retval = remove( "events.lhe" );                  // retval=0 if remove was performed

  //+++++++++++++++++++++++++++++++++++++++++++++++++
  retval = lhefTrf::initializeTrf();
  while(retval==0){ retval = lhefTrf::executeTrf(); }   // retval=0 if there are events left
  retval = lhefTrf::finalizeTrf();
  //+++++++++++++++++++++++++++++++++++++++++++++++++

  std::cout << std::endl << std::endl << std::endl ;
  std::cout << " INFO :: ProtosLHEF_i :: PROTOS LHEF EVENTS FILE *** events.lhe *** CREATED AND READY TO USE ";
  std::cout << std::endl << std::endl << std::endl ;

  return StatusCode::SUCCESS;
}

StatusCode protos2lhef::execute() {
  return StatusCode::SUCCESS;
}

StatusCode protos2lhef::finalize() {
  return StatusCode::SUCCESS;
}
