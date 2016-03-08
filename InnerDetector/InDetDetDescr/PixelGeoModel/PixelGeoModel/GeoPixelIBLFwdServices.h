/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELIBLFWDSERVICES_H
#define GEOPIXELIBLFWDSERVICES_H

#include "PixelGeoModel/GeoVPixelFactory.h"

#include "PixelGeoModel/GeoPixelStaveSupport.h"
#include "PixelGeoModel/GeoPixelLadder.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"


class GeoTransform;
class GeoPhysVol;

class GeoPixelIBLFwdServices :  public GeoVPixelFactory {

public:  

  GeoPixelIBLFwdServices(int);
  virtual GeoVPhysVol* Build();

  bool isComplexShapeDefined();
  GeoPhysVol* getSupportA() const { return m_supportPhysA; }
  GeoPhysVol* getSupportC() const { return m_supportPhysC; }
  GeoTransform* getSupportTrfA() const { return m_xformSupportA; }
  GeoTransform* getSupportTrfC() const { return m_xformSupportC; }

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

