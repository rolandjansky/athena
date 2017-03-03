/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_IGeo2G4Svc_H
#define GEO2G4_IGeo2G4Svc_H

#ifndef GAUDIKERNEL_IINTERFACE_H
#include "GaudiKernel/IInterface.h"
#endif
#include "Geo2G4SvcBase.h"
#include <string>

class IGeo2G4Svc: virtual public IInterface,
                  virtual public Geo2G4SvcBase {
public:
  static const InterfaceID& intID();
};
inline const InterfaceID& IGeo2G4Svc::intID()
{
  static const InterfaceID _IID("IGeo2G4Svc",1,0);
  return _IID;
}
#endif
