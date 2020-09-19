/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SCT_SERVMATFACTORY_H
#define INDETSERVMATGEOMODEL_SCT_SERVMATFACTORY_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;
class InDetMaterialManager;

#include <string>

class SCT_ServMatFactory : public InDetDD::SubDetectorFactoryBase  {

 public:
  
  // Constructor:
  SCT_ServMatFactory(InDetDD::AthenaComps * athenaComps);

  // Destructor:
  ~SCT_ServMatFactory();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);

 private:

 // Illegal operations:
  const SCT_ServMatFactory & operator=(const SCT_ServMatFactory &right);
  SCT_ServMatFactory(const SCT_ServMatFactory &right);

};

#endif //  INDETSERVMATGEOMODEL_SCT_SERVMATFACTORY_H
