/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoSimpleObject_H
#define InDet_GeoSimpleObject_H

#include "PixelGeoComponent/GeoComponent.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoTransform.h"

#include <vector>
#include <string>

class GeoPhysVol;

namespace InDet {

  class GeoSimpleObject : public GeoComponent {
  public:

    GeoSimpleObject( GeoPhysVol* vol);
    GeoSimpleObject( GeoVPhysVol* vol);
    GeoSimpleObject( GeoComponent* comp);
    GeoSimpleObject(GeoComponent* comp, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
    GeoSimpleObject(GeoVPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
    GeoSimpleObject(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);

    virtual ~GeoSimpleObject();

    InDet::GeoSimpleObject* place(const HepGeom::Transform3D& transform, int number) const;

  };

} 

#endif
