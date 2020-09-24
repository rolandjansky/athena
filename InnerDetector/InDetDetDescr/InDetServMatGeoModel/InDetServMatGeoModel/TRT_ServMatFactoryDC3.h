/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYDC3_H
#define INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYDC3_H

#include "InDetGeoModelUtils/InDetSubDetectorFactoryBase.h"

class GeoPhysVol;
class GeoShape;
class GeoMaterial;

#include <string>


class TRT_ServMatFactoryDC3 : public InDetDD::SubDetectorFactoryBase {

 public:
  
  // Constructor:
  TRT_ServMatFactoryDC3(InDetDD::AthenaComps * athenaComps, 
			InDetMaterialManager * matManager);
  
  // Destructor:
  ~TRT_ServMatFactoryDC3();
  
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
  const TRT_ServMatFactoryDC3 & operator=(const TRT_ServMatFactoryDC3 &right);
  TRT_ServMatFactoryDC3(const TRT_ServMatFactoryDC3 &right);

};

#endif //  INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYDC3_H
