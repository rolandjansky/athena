/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_H
#define CSC_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {
    class MYSQL;

    class CSC : public Technology {
      public:
        inline CSC(MYSQL& sql, const std::string& s);
        int numOfLayers;
        double innerRadius;
        double totalThickness;
        double nonsisa;
        double honeycombthick, g10thick, wirespacing, anocathodist;
        double gapbetwcathstrips;
        double readoutstripswidth, phistripwidth, floatingstripswidth;
        double rectwasherthick, roxacellwith;
        double roxwirebargap, fullgasgapwirewidth, fullwirefixbarwidth;
        double wirebarposx, wirebarposy, wirebarposz;

        double cathreadoutpitch, phireadoutpitch;
        int nEtastrips, nPhistrips;
    };

    CSC::CSC(MYSQL& mysql, const std::string& s)
        : Technology(mysql, s), numOfLayers(0), innerRadius(0.), totalThickness(0.), nonsisa(0.), honeycombthick(0.), g10thick(0.), wirespacing(0.), anocathodist(0.),
          gapbetwcathstrips(0.), readoutstripswidth(0.), phistripwidth(0.), floatingstripswidth(0.), rectwasherthick(0.), roxacellwith(0.), roxwirebargap(0.),
          fullgasgapwirewidth(0.), fullwirefixbarwidth(0.), wirebarposx(0.), wirebarposy(0.), wirebarposz(0.), cathreadoutpitch(0.), phireadoutpitch(0.), nEtastrips(0),
          nPhistrips(0) {}
} // namespace MuonGM

#endif
