/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSTAVERINGSERVICES_H
#define GEOPIXELSTAVERINGSERVICES_H

#include "GeoVPixelFactory.h"

#include "GeoPixelStaveSupport.h"
#include "GeoPixelLadder.h"

#include "GeoModelKernel/GeoPhysVol.h"

class GeoTransform;

class ATLAS_NOT_THREAD_SAFE GeoPixelStaveRingServices :  public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.

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

