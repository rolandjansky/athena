/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSpacer_H
#define MMSpacer_H

#include "MuonGeoModel/DetectorElement.h"

#include <vector>

class GeoPhysVol;

namespace MuonGM {

    class Cutout;
    class Component;
    class MMSpacerComponent;
    class MYSQL;

    class MMSpacer : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers
        int index;

        MMSpacer(const MYSQL& mysql, Component *s);
        GeoPhysVol *build(const StoredMaterialManager& matManager,
                          const MYSQL& mysql,
                          int minimalgeo);
        GeoPhysVol *build(const StoredMaterialManager& matManager,
                          const MYSQL& mysql,
                          int minimalgeo, int cutoutson,
                          const std::vector<Cutout *>&);
        virtual void print() override;

      private:
        MMSpacerComponent *m_component;
    };

} // namespace MuonGM

#endif
