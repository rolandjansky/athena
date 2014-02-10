/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// Material_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIAL_P1_TRK_H
#define MATERIAL_P1_TRK_H

#include <vector>

namespace Trk
{
    /** @class Material_p1 
            the persistent object is simply a
            std::vector<float> where

            float  x0;
            float  l0;
            float  A;
            float  Z;
            float  rho;
            
            float dEdX; (optional)
            
            elements & fractions as unsigned char - if filled
            
        */
    class Material_p1
    {
      public:
        Material_p1() :
         materialData(5,0.),
         elements(),
         fractions()
        {}
        
         std::vector<float>         materialData;
         std::vector<unsigned char> elements;
         std::vector<unsigned char> fractions;

    };
}

#endif // MATERIAL_P1_TRK_H
