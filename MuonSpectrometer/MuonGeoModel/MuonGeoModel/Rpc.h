/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Rpc_H
#define Rpc_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/RpcComponent.h"

class GeoVFullPhysVol;
namespace MuonGM {
  class MYSQL;
}

namespace MuonGM {

    class Cutout;

    class Rpc : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers
        double idiv;
        double jdiv;
        float y_translation;
        float z_translation;

        Rpc(const MYSQL& mysql, Component *s);
        GeoVFullPhysVol *build();
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              const MYSQL& mysql,
                              int minimalgeo);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              const MYSQL& mysql,
                              int minimalgeo, int cutoutson,
                              const std::vector<Cutout *>&);
        virtual void print() override;
        unsigned int nGasGaps() const;

      private:
        RpcComponent *m_component;
        unsigned int m_nlayers;
    };
    inline unsigned int Rpc::nGasGaps() const { return m_nlayers; }
} // namespace MuonGM

#endif
