/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcLayer_H
#define RpcLayer_H

#include "GeoModelKernel/GeoVPhysVol.h"
#include "MuonGeoModel/DetectorElement.h"

#include <string>
#include <vector>

namespace MuonGM {

    class Rpc;
    class Cutout;
    class MYSQL;

    class RpcLayer : public DetectorElement {

      public:
        double lwidth;
        double llength;
        double thickness;
        double llongWidth; // for trapezoidal layers

        Rpc *m;

        RpcLayer(const std::string& s, Rpc *t);
        GeoVPhysVol *build(const StoredMaterialManager& matManager,
                           const MYSQL& mysql);
        GeoVPhysVol *build(const StoredMaterialManager& matManager,
                           const MYSQL& mysql,
                           int cutoutson,
                           const std::vector<Cutout *>&);
        virtual void print() override;
    };
} // namespace MuonGM

#endif
