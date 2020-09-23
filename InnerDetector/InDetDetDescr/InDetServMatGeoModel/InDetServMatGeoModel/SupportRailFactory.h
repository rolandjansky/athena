/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SUPPORTRAILFACTORY_H
#define INDETSERVMATGEOMODEL_SUPPORTRAILFACTORY_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;

class SupportRailFactory : public InDetDD::SubDetectorFactoryBase {

 public:
  
  // Constructor:
  SupportRailFactory(InDetDD::AthenaComps * athenaComps, 
		     InDetMaterialManager * matManager);
    
  // Destructor:
  ~SupportRailFactory();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother );

 private:  
  
  // Illegal operations:
  const SupportRailFactory & operator=(const SupportRailFactory &right);
  SupportRailFactory(const SupportRailFactory &right);

};

#endif 
