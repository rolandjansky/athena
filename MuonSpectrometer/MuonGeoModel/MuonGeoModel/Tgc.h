/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Tgc_H
#define Tgc_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/TgcComponent.h"

class GeoFullPhysVol;

namespace MuonGM {

    class Cutout;
    class MYSQL;

    class Tgc : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers
        double irad;
        double orad;
        double dphi;
        int index;

        Tgc(const MYSQL& mysql, Component *s);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              const MYSQL& mysql,
                              int minimalgeo);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              const MYSQL& mysql,
                              int minimalgeo, int cutoutson,
                              const std::vector<Cutout *>&);
        virtual void print() override;

      private:
        TgcComponent *m_component;
    };

} // namespace MuonGM

#endif
