/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscMultiLayer_H
#define CscMultiLayer_H

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "MuonGeoModel/DetectorElement.h"

namespace MuonGM {

    class Cutout;
    class MYSQL;

    class CscMultiLayer : public DetectorElement {

      public: // data members
        int nrOfLayers;

        double width;
        double longWidth;
        double upWidth;

        double excent;
        double length;
        double physicalLength;
        double maxwLength;

        double thickness;
        double cscthickness;
        double dim[8]{};

      public: // methods
        CscMultiLayer(const MYSQL& mysql, const std::string& n);
        GeoVPhysVol *build(const StoredMaterialManager& matManager,
                           const MYSQL& mysql);
        GeoVPhysVol *build(const StoredMaterialManager& matManager,
                           const MYSQL& mysql,
                           int cutoutson,
                           const std::vector<Cutout *>& vcutdef);
        virtual void print() override;
    };

} // namespace MuonGM

#endif
