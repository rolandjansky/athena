/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_ENDPLATEFACTORY_H
#define INDETSERVMATGEOMODEL_ENDPLATEFACTORY_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;

class EndPlateFactory : public InDetDD::SubDetectorFactoryBase  {

 public:
  
  // Constructor:
  EndPlateFactory(const InDetDD::AthenaComps * athenaComps, 
		  InDetMaterialManager * matManager);
  
  // Destructor:
  ~EndPlateFactory();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother );

 private:  
  
  // Illegal operations:
  const EndPlateFactory & operator=(const EndPlateFactory &right);
  EndPlateFactory(const EndPlateFactory &right);

};

#endif 
