/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SQUIRRELCAGEFACTORY_H
#define INDETSERVMATGEOMODEL_SQUIRRELCAGEFACTORY_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;

class SquirrelCageFactory : public InDetDD::SubDetectorFactoryBase  {

 public:
  
  // Constructor:
  SquirrelCageFactory(InDetDD::AthenaComps * athenaComps, 
		      InDetMaterialManager * matManager);
  
  // Destructor:
  ~SquirrelCageFactory();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother );


 private:  
  
  // Illegal operations:
  const SquirrelCageFactory & operator=(const SquirrelCageFactory &right);
  SquirrelCageFactory(const SquirrelCageFactory &right);

};

#endif 
