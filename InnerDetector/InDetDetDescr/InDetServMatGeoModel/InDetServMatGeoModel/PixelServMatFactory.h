/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_PIXELSERVMATFACTORY_H
#define INDETSERVMATGEOMODEL_PIXELSERVMATFACTORY_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;
class InDetMaterialManager;

class PixelServMatFactory  : public InDetDD::SubDetectorFactoryBase  {

 public:
  
  // Constructor:
  PixelServMatFactory(InDetDD::AthenaComps * athenaComps);
  
  // Destructor:
  ~PixelServMatFactory();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);

 private:  
  
  // Illegal operations:
  const PixelServMatFactory & operator=(const PixelServMatFactory &right);
  PixelServMatFactory(const PixelServMatFactory &right);

};

#endif //  INDETSERVMATGEOMODEL_PIXELSERVMATFACTORY_H
