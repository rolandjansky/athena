/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IPseudoJetGetter.cxx

#include "JetInterface/IPseudoJetGetter.h"

//**********************************************************************

const PseudoJetVector* IPseudoJetGetter::get() const {
  return 0;
}

//**********************************************************************

int IPseudoJetGetter::inputContainerNames(std::vector<std::string>&) {
  return 0;
}

//**********************************************************************
//
int IPseudoJetGetter::outputContainerNames(std::vector<std::string>&) {
  return 0;
}

//**********************************************************************
