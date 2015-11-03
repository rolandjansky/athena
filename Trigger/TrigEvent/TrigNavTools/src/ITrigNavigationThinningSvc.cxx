/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavTools/ITrigNavigationThinningSvc.h"


ITrigNavigationThinningSvc::~ITrigNavigationThinningSvc(){}

const InterfaceID& ITrigNavigationThinningSvc::interfaceID() 
{ 
  static const InterfaceID IID("ITrigNavigationThinningSvc", 1, 0);
  return IID; 
}



