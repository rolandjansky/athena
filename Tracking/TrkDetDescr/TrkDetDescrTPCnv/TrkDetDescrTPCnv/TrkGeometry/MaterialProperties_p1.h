/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialProperties_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIALPROPERTIES_P1_TRK_H
#define MATERIALPROPERTIES_P1_TRK_H

#include <vector>

namespace Trk
{
    /** @class MaterialProperties_p1 
            the persistent object is simply a
            std::vector<float> where

            float  x0;
            float  l0;
            float  A;
            float  Z;
            float  rho;
            float  d;   // material thickness in mm 
            
            float dEdX; (optional)
            
            elements & fractions as unsigned char - if filled
            
        */
    class MaterialProperties_p1
    {
      public:
        MaterialProperties_p1() :
         materialData(6,0.),
         elements(),
         fractions()
        {}
        
         std::vector<float>         materialData;
         std::vector<unsigned char> elements;
         std::vector<unsigned char> fractions;

    };
}

#endif // MATERIALPROPERTIES_P1_TRK_H
