/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELIFLEXSERVICES_H
#define GEOPIXELIFLEXSERVICES_H

#include "PixelGeoModel/GeoVPixelFactory.h"

#include "PixelGeoModel/GeoPixelStaveSupport.h"
#include "PixelGeoModel/GeoPixelLadder.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"


class GeoTransform;
class GeoPhysVol;

class GeoPixelIFlexServices :  public GeoVPixelFactory {

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

