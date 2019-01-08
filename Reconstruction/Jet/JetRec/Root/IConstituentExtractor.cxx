/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "JetRec/IConstituentExtractor.h"

IConstituentExtractor::~IConstituentExtractor(){}

std::ostream& operator << (std::ostream& os, const IConstituentExtractor* e){
  os <<  e->toString(0);
  return os;
}
