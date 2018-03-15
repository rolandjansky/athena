/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoStaveRingSupportAlpine_H
#define InDet_GeoStaveRingSupportAlpine_H

#include "PixelGeoComponent/GeoComponent.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoTransform.h"

#include <vector>
#include <string>

class GeoFullPhysVol;
class GeoPhysVol;

namespace InDet {

  class GeoStaveRingSupportAlpine : public GeoComponent {
  public:

    GeoStaveRingSupportAlpine( GeoFullPhysVol* vol);
    GeoStaveRingSupportAlpine( GeoPhysVol* vol);
    GeoStaveRingSupportAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);

      //    virtual ~GeoStaveRingSupportAlpine();

 /*    InDet::GeoStaveRingSupportAlpine* copyandclone() const;  */
/*     InDet::GeoStaveRingSupportAlpine*  copyandclone(const HepGeom::Transform3D& transform, int number) const; */
    InDet::GeoStaveRingSupportAlpine* place(const HepGeom::Transform3D& transform, int number) const;


  };

} 

#endif
