/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IGeo2G4Svc_H
#define IGeo2G4Svc_H

#ifndef GAUDIKERNEL_IINTERFACE_H
 #include "GaudiKernel/IInterface.h"
#endif

#include <string>

class VolumeBuilder;

class IGeo2G4Svc: virtual public IInterface {
public:
	static const InterfaceID& intID();
};
inline const InterfaceID& IGeo2G4Svc::intID()
{
	static const InterfaceID _IID("IGeo2G4Svc",1,0);
	return _IID;
}
#endif
