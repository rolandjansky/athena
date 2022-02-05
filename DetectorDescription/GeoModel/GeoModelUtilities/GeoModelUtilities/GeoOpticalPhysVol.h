/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoOpticalPhysVol_h
#define GeoOpticalPhysVol_h 1

/// Ensure that the extensions for the Vector3D are properly loaded
#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoPhysVol.h"

class GeoOpticalPhysVol : public GeoPhysVol
{
 public:
  GeoOpticalPhysVol(const GeoLogVol* LogVol);

 protected:
  ~GeoOpticalPhysVol();

 private:
  GeoOpticalPhysVol(const GeoOpticalPhysVol &right);
  GeoOpticalPhysVol & operator=(const GeoOpticalPhysVol &right);

};

#endif 
