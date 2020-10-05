/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELIBLFWDSERVICES_H
#define GEOPIXELIBLFWDSERVICES_H

#include "GeoVPixelFactory.h"

#include "GeoPixelStaveSupport.h"
#include "GeoPixelLadder.h"

#include "GeoModelKernel/GeoPhysVol.h"

class GeoTransform;

class GeoPixelIBLFwdServices :  public GeoVPixelFactory {

public:  

  GeoPixelIBLFwdServices(InDetDD::PixelDetectorManager* ddmgr,
                         PixelGeometryManager* mgr, int);
  virtual GeoVPhysVol* Build() override;

  bool isComplexShapeDefined();
  GeoPhysVol* getSupportA() { return m_supportPhysA; }
  GeoPhysVol* getSupportC() { return m_supportPhysC; }
  GeoTransform* getSupportTrfA() { return m_xformSupportA; }
  GeoTransform* getSupportTrfC() { return m_xformSupportC; }

 private:

  bool m_bFwdComplexGeometry_CAD;
  bool m_bFwdComplexGeometry_Mod1;

  int m_section;
  GeoPhysVol *m_supportPhysA;
  GeoPhysVol *m_supportPhysC;
  GeoTransform *m_xformSupportA;
  GeoTransform *m_xformSupportC;

};

#endif

