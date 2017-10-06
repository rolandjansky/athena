// Dear emacs, this is -*- c++ -*-
#include "ALP/ALP.h"

extern "C" hltinterface::HLTInterface* create_interface(){
  return new ALP();
}

extern "C" void destroy_interface(hltinterface::HLTInterface* i){
  auto h=dynamic_cast<ALP*>(i);
  delete h;
}
