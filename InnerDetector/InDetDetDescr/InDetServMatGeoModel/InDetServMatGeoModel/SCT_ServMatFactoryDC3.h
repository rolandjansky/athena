/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SCT_SERVMATFACTORYDC3_H
#define INDETSERVMATGEOMODEL_SCT_SERVMATFACTORYDC3_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;

class GeoPhysVol;
class GeoShape;
class GeoMaterial;

#include <string>

class SCT_ServMatFactoryDC3 : public InDetDD::SubDetectorFactoryBase  {

 public:
  
  // Constructor:
  SCT_ServMatFactoryDC3(const InDetDD::AthenaComps * athenaComps, 
			InDetMaterialManager * matManager);
  
  // Destructor:
  virtual ~SCT_ServMatFactoryDC3() = default;
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);


 private:  
  const GeoShape* createShape(int volType, 
			      double rmin1, 
			      double rmax1, 
			      double halflength,
			      double rmin2,
			      double rmax2);
  
  const GeoMaterial* createMaterial(const std::string & name,
				    int volType, 
				    double fractionRL,
				    double rmin1, 
				    double rmax1, 
				    double halflength,
				    double rmin2,
				    double rmax2);

 // Illegal operations:
  const SCT_ServMatFactoryDC3 & operator=(const SCT_ServMatFactoryDC3 &right);
  SCT_ServMatFactoryDC3(const SCT_ServMatFactoryDC3 &right);

};

#endif //  INDETSERVMATGEOMODEL_SCT_SERVMATFACTORYDC3_H
