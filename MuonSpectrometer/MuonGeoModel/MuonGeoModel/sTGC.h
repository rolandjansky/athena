/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_H
#define sTGC_H

#include "MuonGeoModel/DetectorElement.h"

#include <vector>

class GeoFullPhysVol;

namespace MuonGM {

    class Cutout;
    class Component;
    class sTGCComponent;

    class sTGC : public DetectorElement {

      public:
        double width;
        double length;
        double thickness = 0.0;
        double longWidth;      // for trapezoidal layers
        double yCutout;        // for Hexagonal layer
        double yCutoutCathode; // for Hexagonal layer
        int index;

        sTGC(Component *s);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              int minimalgeo);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              int minimalgeo, int cutoutson,
                              const std::vector<Cutout *>&);
        virtual void print() override;

      private:
        sTGCComponent *m_component;
    };

} // namespace MuonGM

#endif
