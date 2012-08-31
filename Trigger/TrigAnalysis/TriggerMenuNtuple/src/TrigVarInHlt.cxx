/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInHlt_cxx__
#define __TrigVarInHlt_cxx__
/*
  TrigVarInHlt.cxx
*/

#include "TriggerMenuNtuple/TrigVarInHlt.h"


TrigVarInHlt::TrigVarInHlt(std::string chain_name, bool ActiveOnly) :
  mChainName(chain_name), mActiveOnly(ActiveOnly) {
}

TrigVarInHlt::~TrigVarInHlt(){
}


#endif // __TrigVarInHlt_cxx__
