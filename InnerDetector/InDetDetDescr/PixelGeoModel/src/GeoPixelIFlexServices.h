/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELIFLEXSERVICES_H
#define GEOPIXELIFLEXSERVICES_H

#include "GeoVPixelFactory.h"

#include "GeoPixelStaveSupport.h"
#include "GeoPixelLadder.h"

#include "GeoModelKernel/GeoPhysVol.h"

class GeoTransform;

class ATLAS_NOT_THREAD_SAFE GeoPixelIFlexServices :  public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.

public:  

  GeoPixelIFlexServices(int iSection);
  virtual GeoVPhysVol* Build();

  GeoPhysVol* getSupportA(){ return m_supportPhysA; }
  GeoPhysVol* getSupportC(){ return m_supportPhysC; }
  GeoTransform* getSupportTrfA(){ return m_xformSupportA; }
  GeoTransform* getSupportTrfC(){ return m_xformSupportC; }

 private:

  double m_section;
  GeoPhysVol *m_supportPhysA;
  GeoPhysVol *m_supportPhysC;
  GeoTransform *m_xformSupportA;
  GeoTransform *m_xformSupportC;

};

#endif

