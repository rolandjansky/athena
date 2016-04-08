/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMCNVSVCBASE_IMUONCALIBSTREAMEVENTACCESS_H
#define MUONCALIBSTREAMCNVSVCBASE_IMUONCALIBSTREAMEVENTACCESS_H

#include "GaudiKernel/IInterface.h"

static const InterfaceID
IID_IMuonCalibStreamEventAccess("IMuonCalibStreamEventAccess", 2 , 0);

class IMuonCalibStreamEventAccess: virtual public IInterface {
 public:
  static const InterfaceID& interfaceID() { 
    return IID_IMuonCalibStreamEventAccess; 
  }
};

#endif 
