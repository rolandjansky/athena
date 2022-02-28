/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_INDETSERVMATDETECTORFACTORY_LITE_H
#define INDETSERVMATGEOMODEL_INDETSERVMATDETECTORFACTORY_LITE_H

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "InDetServMatGeoModel/InDetServMatManager.h"
#include <string>


class InDetServMatFactory_Lite final : public GeoVDetectorFactory  
{
 public:
  
  // Constructor:
  InDetServMatFactory_Lite();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world) override;
  
  // Access to the results:
  virtual const InDetDD::InDetServMatManager * getDetectorManager() const override;
  

 private:  

  // Illegal operations:
  const InDetServMatFactory_Lite & operator=(const InDetServMatFactory_Lite &right);
  InDetServMatFactory_Lite(const InDetServMatFactory_Lite &right);
  
  // The manager:
  InDetDD::InDetServMatManager     * m_manager;

};

// Class InDetServMatFactory_Lite 
#endif


