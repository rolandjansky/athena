/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_TRT_SERVMATFACTORY_H
#define INDETSERVMATGEOMODEL_TRT_SERVMATFACTORY_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;
class InDetMaterialManager;

#include <string>

class TRT_ServMatFactory : public InDetDD::SubDetectorFactoryBase  {

 public:
  
  // Constructor:
  TRT_ServMatFactory(InDetDD::AthenaComps * athenaComps);

  // Destructor:
  ~TRT_ServMatFactory();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);

 private:

  // Illegal operations:
  const TRT_ServMatFactory & operator=(const TRT_ServMatFactory &right);
  TRT_ServMatFactory(const TRT_ServMatFactory &right);

};

#endif //  INDETSERVMATGEOMODEL_TRT_SERVMATFACTORY_H
