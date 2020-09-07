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

class GeoPixelIFlexServices :  public GeoVPixelFactory {

public:  

  GeoPixelIFlexServices(InDetDD::PixelDetectorManager* ddmgr,
                        PixelGeometryManager* mgr, int iSection);
  virtual GeoVPhysVol* Build() override;

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

