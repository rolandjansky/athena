/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_H
#define CSC_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {

class CSC: public Technology {
public:
    inline CSC(std::string s);
    int numOfLayers;
    double innerRadius;
    double totalThickness;
    double nonsisa;
    double honeycombthick,g10thick,wirespacing,anocathodist;
    double gapbetwcathstrips;
    double readoutstripswidth, phistripwidth, floatingstripswidth;
    double rectwasherthick, roxacellwith;
    double roxwirebargap,fullgasgapwirewidth, fullwirefixbarwidth;
    double wirebarposx,wirebarposy,wirebarposz;

    double cathreadoutpitch, phireadoutpitch;
    int nEtastrips, nPhistrips;
};

CSC::CSC(std::string s): Technology(s)
{
}
} // namespace MuonGM

#endif
