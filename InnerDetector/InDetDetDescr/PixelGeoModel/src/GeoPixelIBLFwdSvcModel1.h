/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELIBLFWDSVCMODEL1_H
#define GEOPIXELIBLFWDSVCMODEL1_H

#include "GeoVPixelFactory.h"

#include "GeoPixelStaveSupport.h"
#include "GeoPixelLadder.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"

class GeoTransform;

class GeoPixelIBLFwdSvcModel1 :  public GeoVPixelFactory {

public:  

  GeoPixelIBLFwdSvcModel1(InDetDD::PixelDetectorManager* ddmgr,
                          PixelGeometryManager* mgr, int);
  virtual GeoVPhysVol* Build() override;

  GeoPhysVol* getSupportA() { return m_supportPhysA; }
  GeoPhysVol* getSupportC() { return m_supportPhysC; }
  GeoTransform* getSupportTrfA() { return m_xformSupportA; }
  GeoTransform* getSupportTrfC() { return m_xformSupportC; }

 private:

  GeoPhysVol *m_supportPhysA;
  GeoPhysVol *m_supportPhysC;
  GeoTransform *m_xformSupportA;
  GeoTransform *m_xformSupportC;

  const GeoShape* addShape(const GeoShape * lastShape, const GeoShape * nextShape, const GeoTrf::Transform3D & trans);
};

#endif

