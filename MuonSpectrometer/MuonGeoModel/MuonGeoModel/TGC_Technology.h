/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_H
#define TGC_H

#include "MuonGeoModel/Technology.h"

#include <vector>
namespace MuonGM {

    class TGC : public Technology {
      public:
        //	double thickness;
        int nlayers;
        double frame_h;
        double frame_ab;
        std::vector<std::string> materials;
        std::vector<double> positions;
        std::vector<double> tck;

        // inner structure parameters from GGLN
        // For wire supports
        double widthWireSupport;     // width of wire support, GGLN/S1PP
        double widthGasChannel;      // not used, GGLN/S2PP
        double distanceWireSupport;  // distance between two neigbouring wire supports, GGLN/WSEP
        double offsetWireSupport[3]; // offset w.r.t the chamber centre axis for each layer, GGLN/SP[1-3]WI
        double angleTilt;            // tilt angle of wire support, GGLN/TILT
        // For button supports
        double radiusButton;   // radius of a button support, GGLN/SP1BU
        double pitchButton[2]; // pitch in y and z axies, GGLN/SP[2,3]BU
        double angleButton;    // tilt angle in trapezoid regions, GGLN/SP4BU

        inline TGC(std::string s);
    };

    TGC::TGC(std::string s)
        : Technology(s), nlayers(0), frame_h(0.), frame_ab(0.), widthWireSupport(0.), widthGasChannel(0.), distanceWireSupport(0.), angleTilt(0.), radiusButton(0.),
          angleButton(0.) {}
} // namespace MuonGM

#endif
