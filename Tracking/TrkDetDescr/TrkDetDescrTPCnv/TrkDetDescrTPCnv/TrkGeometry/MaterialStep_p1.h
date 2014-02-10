/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStep_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIALSTEP_P1_TRK_H
#define MATERIALSTEP_P1_TRK_H

#include <vector>

namespace Trk
{
    /** @class MaterialStep_p1 
            the persistent object is simply a
            std::vector<float> where
            
            float  steplength;
                   
            float  hitX;
            float  hitY;
            float  hitZ;

            float  x0;
            float  l0;
            float  A;
            float  Z;
            float  rho;
            
            unsigned char vector for elements
            unsigned char vector for weights
        */
    class MaterialStep_p1
    {
      public:
        MaterialStep_p1() :
         materialData(9,0.) {}
        
         std::vector<float> materialData;
         
         std::vector<unsigned char> elements;
         std::vector<unsigned char> fractions;
         

    };
}

#endif // MATERIALSTEP_P1_TRK_H