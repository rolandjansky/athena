/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_INDETSERVMATFACTORY_H
#define INDETSERVMATGEOMODEL_INDETSERVMATFACTORY_H


#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "InDetServMatGeoModel/InDetServMatManager.h"

namespace InDetDD {
  class AthenaComps;
}

class InDetServMatFactory : public InDetDD::DetectorFactoryBase  {

 public:
  
  // Constructor:
  InDetServMatFactory(InDetDD::AthenaComps * athenaComps);

  // Destructor:
  ~InDetServMatFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  // manager
  virtual const InDetDD::InDetServMatManager* getDetectorManager () const;

private:  
  
  // Illegal operations:
  const InDetServMatFactory & operator=(const InDetServMatFactory &right);
  InDetServMatFactory(const InDetServMatFactory &right);

  // private data
  InDetDD::InDetServMatManager   *m_manager;
};

#endif //  INDETSERVMATGEOMODEL_INDETSERVMATFACTORY_H


