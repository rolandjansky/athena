/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Cutout_H
#define Cutout_H

#include <iostream>

class GeoShape;

namespace MuonGM {

class Cutout {
public:

    Cutout();
    Cutout(const Cutout& c);
    void setThickness(double compThickness);
    const GeoShape* build();

    int ijob;
    int subtype;
    int icut;
    double dx;
    double dy;
    double widthXs;
    double widthXl;
    double lengthY;
    double excent;
    double dead1;
    double thickness;

    friend std::ostream& operator<<(std::ostream &os, const Cutout& p);

private:
    Cutout & operator=(const Cutout &right);
};
} // namespace MuonGM

#endif
