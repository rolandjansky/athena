/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGEOMODEL_GEOPIXELLAYER_H
#define PIXELGEOMODEL_GEOPIXELLAYER_H

#include "GeoVPixelFactory.h"
#include "InDetGeoModelUtils/GeoNodePtr.h"

class GeoPhysVol;
class GeoTransform;

class GeoPixelLayer : public GeoVPixelFactory {

 public:
  GeoPixelLayer(InDetDD::PixelDetectorManager* ddmgr,
                PixelGeometryManager* mgr);
  virtual GeoVPhysVol* Build() override;

  GeoPhysVol* getSupportA(){ return m_supportPhysA; }
  GeoPhysVol* getSupportC(){ return m_supportPhysC; }
  GeoVPhysVol* getSupportMidRing(){ return m_supportMidRing; }

  GeoTransform* getSupportTrfA(){ return m_xformSupportA; }
  GeoTransform* getSupportTrfC(){ return m_xformSupportC; }
  GeoTransform* getSupportTrfMidRing(){ return m_xformSupportMidRing; }

 private:
  GeoNodePtr<GeoPhysVol> m_supportPhysA;
  GeoNodePtr<GeoPhysVol> m_supportPhysC;
  GeoNodePtr<GeoVPhysVol> m_supportMidRing;

  GeoNodePtr<GeoTransform> m_xformSupportA;
  GeoNodePtr<GeoTransform> m_xformSupportC;
  GeoNodePtr<GeoTransform> m_xformSupportMidRing;

};

#endif // not PIXELGEOMODEL_GEOPIXELLAYER_H
