/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IT2CaloInitSvc_H
#define IT2CaloInitSvc_H

#include "GaudiKernel/IInterface.h"

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IT2CaloInitSvc("IT2CaloInitSvc", 1, 0);

class IT2CaloInitSvc : virtual public IInterface {

    public:

    static const InterfaceID& interfaceID() { return IID_IT2CaloInitSvc; }

};

#endif



