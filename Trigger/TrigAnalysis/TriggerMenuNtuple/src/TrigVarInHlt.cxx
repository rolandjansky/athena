/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigVarInHlt.cxx
*/

#include "TriggerMenuNtuple/TrigVarInHlt.h"


TrigVarInHlt::TrigVarInHlt(std::string chain_name, bool ActiveOnly) :
  m_chainName(chain_name), m_activeOnly(ActiveOnly) {
}

TrigVarInHlt::~TrigVarInHlt(){
}


