/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUP_H
#define SUP_H

#include "MuonGeoModel/Technology.h"

#include <string>
namespace MuonGM {

    class SUP : public Technology {
      public:
        // ss{
        // CARTEC [6] =SUP   TT -> statt; OO->STAOO
        // TT 0 0 2 1 96 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0
        // OO 0 0 50 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0
        //=>for sup1 xx1=2, xx2=96;
        //           zz1=50
        // TT 20 20 2 21 96 22 0 23 0 24 0 25 0 26 0 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0
        // OO 20 20 -50 21 0 22 0 23 0 24 0 25 0 26 0 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0
        // =>for sup1 xx1=2, xx2=96;
        //            zz1=-50
        // TT 40 40 2 41 8 42 46 43 50 44 0 45 0 46 0 47 0 48 0 49 0 50 0 51 0 52 0 53 0 54 0 55 0 56 0 57 0 58 0 59 0
        // OO 40 40 2 41 50 42 64 43 4 44 0 45 0 46 0 47 0 48 0 49 0 50 0 51 0 52 0 53 0 54 0 55 0 56 0 57 0 58 0 59 0
        //  =>for sup3 xx1=2, xx2=8, xx3=46, xx4=50;
        //             zz1=2, zz2=50, zz3=64, zz4=4;
        // picture in dice/muons/rpcsup.txt

        double alFlangeThickness;     // XX1
        double alHorFlangeLength;     // ZZ1 for sup1 and sup2     //ZZ2     for sup3
        double alVerFlangeLength;     // 0   for sup1 and sup2     //XX2-XX1 for sup3
        double alVerProfileThickness; // XX1 for sup1 and sup2     //ZZ4     for sup3
        double alHorProfileThickness; // 0(not defined) for sup1,2 //XX4-XX3 for sup3
        double largeVerClearance;     // XX2 for sup1 and sup2     //XX4     for sup3
        double smallVerClearance;     // 0(not defined) for sup1,2 //XX3     for sup3
        double HorClearance;          // 0(not defined) for sup1,2 //ZZ3     for sup3
        double totalThickness;

        double xAMDB0, yAMDB0, zAMDB0;
        double bottomsizewrtAMDB0, topsizewrtAMDB0;
        // ss}

        double alThickness;
        double length;
        double sup3Square, sup3VerticalThickness, sup3CentralLength, sup3OuterLength; // only for SUP3

        //    double thickness() const;
        SUP(std::string s);
    };
} // namespace MuonGM

#endif
