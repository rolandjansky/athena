/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Rpc_H
#define Rpc_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/RpcComponent.h"

class GeoVFullPhysVol;

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

        Rpc(Component *s);
        GeoVFullPhysVol *build();
        GeoFullPhysVol *build(int minimalgeo);
        GeoFullPhysVol *build(int minimalgeo, int cutoutson, std::vector<Cutout *>);
        void print();
        unsigned int nGasGaps() const;

      private:
        RpcComponent *m_component;
        unsigned int m_nlayers;
    };
    inline unsigned int Rpc::nGasGaps() const { return m_nlayers; }
} // namespace MuonGM

#endif
