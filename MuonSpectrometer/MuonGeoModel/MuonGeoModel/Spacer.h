/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Spacer_H
#define Spacer_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/SpaComponent.h"

class GeoVPhysVol;

namespace MuonGM {
    class MYSQL;

    class Spacer : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers

        Spacer(const MYSQL& mysql, Component *s);
        GeoVPhysVol *build(const StoredMaterialManager& matManager);
        GeoVPhysVol *build(const StoredMaterialManager& matManager, int cutoutson);
        virtual void print() override;

      private:
        SpaComponent m_component;
    };

} // namespace MuonGM

#endif
