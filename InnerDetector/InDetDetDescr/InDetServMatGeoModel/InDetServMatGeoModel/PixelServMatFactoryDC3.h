/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC3_H
#define INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC3_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"
class GeoPhysVol;

class PixelServMatFactoryDC3 : public InDetDD::SubDetectorFactoryBase  {

 public:
  
  // Constructor:
  PixelServMatFactoryDC3(InDetDD::AthenaComps * athenaComps, 
			 InDetMaterialManager * matManager);

  
  // Destructor:
  ~PixelServMatFactoryDC3();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);

 private:  
  
  // Illegal operations:
  const PixelServMatFactoryDC3 & operator=(const PixelServMatFactoryDC3 &right);
  PixelServMatFactoryDC3(const PixelServMatFactoryDC3 &right);

};

#endif //  INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC3_H
