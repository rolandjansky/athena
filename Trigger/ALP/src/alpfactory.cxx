// Dear emacs, this is -*- c++ -*-
/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/
#include "ALP/ALP.h"

extern "C" hltinterface::HLTInterface* create_interface(){
  return new ALP();
}

extern "C" void destroy_interface(hltinterface::HLTInterface* i){
  auto h=dynamic_cast<ALP*>(i);
  delete h;
}
