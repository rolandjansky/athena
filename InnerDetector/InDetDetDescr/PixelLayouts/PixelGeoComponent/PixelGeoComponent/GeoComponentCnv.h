/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoComponentCnv_H
#define InDet_GeoComponentCnv_H

#include "PixelGeoComponent/GeoComponent.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoTransform.h"

#include <vector>
#include <string>

class GeoPhysVol;
class GeoFullPhysVol;
class GeoVPhysVol;

namespace InDet {

  class GeoComponent;
  class GeoSimpleObject;

  class GeoComponentCnv {
  public:

    GeoComponentCnv( );
    virtual ~GeoComponentCnv();

    GeoSimpleObject* convertIntoGeoComponent(const GeoVPhysVol *vol) const;
    GeoSimpleObject* convertIntoGeoComponent(const GeoVPhysVol *vol,  const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag)  const;

  private:
    GeoComponent* browsePhysVolTree(const GeoVPhysVol* vol, int iLevel=0) const;

  };

} 

#endif
