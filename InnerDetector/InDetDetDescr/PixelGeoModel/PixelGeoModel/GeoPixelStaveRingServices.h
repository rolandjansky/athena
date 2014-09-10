/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSTAVERINGSERVICES_H
#define GEOPIXELSTAVERINGSERVICES_H

#include "PixelGeoModel/GeoVPixelFactory.h"

#include "PixelGeoModel/GeoPixelStaveSupport.h"
#include "PixelGeoModel/GeoPixelLadder.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"


class GeoTransform;
class GeoPhysVol;

class GeoPixelStaveRingServices :  public GeoVPixelFactory {

public:  

  GeoPixelStaveRingServices(GeoPixelLadder& ladder, GeoPixelStaveSupport& staveSupport);
  virtual GeoVPhysVol* Build();

  GeoPhysVol* getSupportA(){ return m_supportPhysA; }
  GeoPhysVol* getSupportC(){ return m_supportPhysC; }
  GeoVPhysVol* getSupportMidRing(){ return m_supportMidRing; }

  GeoTransform* getSupportTrfA(){ return m_xformSupportA; }
  GeoTransform* getSupportTrfC(){ return m_xformSupportC; }
  GeoTransform* getSupportTrfMidRing(){ return m_xformSupportMidRing; }

 private:

  GeoPixelLadder& m_ladder;
  GeoPixelStaveSupport& m_staveSupport;  

  GeoPhysVol *m_supportPhysA;
  GeoPhysVol *m_supportPhysC;
  GeoVPhysVol *m_supportMidRing;

  GeoTransform *m_xformSupportA;
  GeoTransform *m_xformSupportC;
  GeoTransform *m_xformSupportMidRing;


};

#endif

