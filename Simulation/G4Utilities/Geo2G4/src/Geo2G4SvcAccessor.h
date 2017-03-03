/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4SvcAccessor_H
#define GEO2G4_Geo2G4SvcAccessor_H

#include "G4AtlasInterfaces/Geo2G4SvcBase.h"
#include "G4AtlasInterfaces/IGeo2G4Svc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

class Geo2G4SvcAccessor {
public:
  Geo2G4SvcBase* GetGeo2G4Svc() const;
};

inline Geo2G4SvcBase* Geo2G4SvcAccessor::GetGeo2G4Svc() const
{
  IService* g2gSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode result = svcLocator->service("Geo2G4Svc",g2gSvc);

  if(result.isSuccess())
    {
      Geo2G4SvcBase* temp=dynamic_cast<Geo2G4SvcBase*>(g2gSvc);
      return temp;
    }
  else
    return 0;
}

#endif
