/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/ITimeKeeper.h"
const InterfaceID& 
ITimeKeeper::interfaceID() {
  static const InterfaceID _IID("ITimeKeeper", 1, 0);
  return _IID;
}
