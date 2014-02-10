/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BinUtility_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BINUTILITY_P1_TRK_H
#define BINUTILITY_P1_TRK_H

#include <vector>

namespace Trk
{
    /** @class BinUtility_p1 
        */
    class BinUtility_p1
    {
      public:
        BinUtility_p1() :
          type(),
          option(),
          binvalue(),
          bins(),
          min(),
          max(),
          step(),
          subStep(),
          boundaries(),
          refphi(),    
          hbounds() 
        {} 
        
         std::vector<int>                                  type;
         std::vector<int>                                  option;
         std::vector<int>                                  binvalue;
         std::vector<unsigned int>                         bins;
         std::vector<float>                                min;
         std::vector<float>                                max;
         std::vector<float>                                step;
         std::vector<float>                                subStep;
         std::vector< std::vector<float> >                 boundaries;
         std::vector<float>                                refphi;    // ref Phi for binH
         std::vector< std::vector<std::pair<int,float> > > hbounds;   // boundary for binH
         
    };
}

#endif // GENERICBINUTILITY_P1_TRK_H