/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// HomogeneousLayerMaterial_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HOMOGENEOUSLAYERMATERIAL_P1_TRK_H
#define HOMOGENEOUSLAYERMATERIAL_P1_TRK_H

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
    /** @class HomogeneousLayerMaterial_p1 
        */
    class HomogeneousLayerMaterial_p1
    {
      public:
        HomogeneousLayerMaterial_p1() :
          splitFactor(1.)
        {}
        
        TPObjRef   material;
        float      splitFactor;
        

    };
}

#endif // BINNEDLAYERMATERIAL_P1_TRK_H