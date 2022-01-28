/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMPIPEGEOMODEL_BEAMPIPEDETECTORFACTORY_LITE_H
#define BEAMPIPEGEOMODEL_BEAMPIPEDETECTORFACTORY_LITE_H

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "BeamPipeGeoModel/BeamPipeDetectorManager.h"
#include <string>


class BeamPipeDetectorFactory_Lite final : public GeoVDetectorFactory  
{
 public:
  
  // Constructor:
  BeamPipeDetectorFactory_Lite();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world) override;
  
  // Access to the results:
  virtual const BeamPipeDetectorManager * getDetectorManager() const override;
  

 private:  

  // Illegal operations:
  const BeamPipeDetectorFactory_Lite & operator=(const BeamPipeDetectorFactory_Lite &right);
  BeamPipeDetectorFactory_Lite(const BeamPipeDetectorFactory_Lite &right);
  
  // The manager:
  BeamPipeDetectorManager     * m_manager;

};

// Class BeamPipeDetectorFactory_Lite 
#endif


