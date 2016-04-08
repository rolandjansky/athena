/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ProtosLHEF_i/lhefTrf.h"
#include <iostream>

extern "C" {
  void initprotos_();
  void openlhef_();
  void useprotos_();
  void writelhef_(int*retval);
  void endlhef_();
}

lhefTrf::lhefTrf() {}

int lhefTrf::initializeTrf() {

  initprotos_();
  openlhef_();

  return 0;
}

int lhefTrf::executeTrf() {

  int retval = 0;
  useprotos_();
  writelhef_(&retval);
  
  return retval;
}

int lhefTrf::finalizeTrf() {

  endlhef_();

  return 0;
}
