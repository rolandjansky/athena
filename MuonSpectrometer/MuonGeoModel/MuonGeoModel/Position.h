/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Position_H
#define Position_H

#include <iostream>
namespace MuonGM {

    class Position {
      public:
        Position();

        int zindex;
        int phiindex;
        int phitype;
        int icut;
        int subtype;
        double phi;
        double radius;
        double z;
        double shift;
        double inclination;
        double alpha;
        double beta;
        double gamma;

        bool isAssigned;

        bool isMirrored;
        bool isBarrelLike;
        friend std::ostream &operator<<(std::ostream &os, const Position &p);
    };
} // namespace MuonGM

#endif
