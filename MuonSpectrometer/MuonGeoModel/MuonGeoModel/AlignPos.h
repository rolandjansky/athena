/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AlignPos_H
#define AlignPos_H

#include <iostream>
#include <string>
namespace MuonGM {

    class AlignPos {
      public:
        AlignPos();

        int zindex;
        int phiindex;
        int jobindex;
        double tras;
        double traz;
        double trat;
        double rots;
        double rotz;
        double rott;
        bool isBarrel;
        bool isTrapezoid;    // yuck! GeoModel axes different for box, trap
        std::string tectype; // eg BOS1 (to match to station type)
        friend std::ostream &operator<<(std::ostream &os, const AlignPos &p);
    };

} // namespace MuonGM
#endif
