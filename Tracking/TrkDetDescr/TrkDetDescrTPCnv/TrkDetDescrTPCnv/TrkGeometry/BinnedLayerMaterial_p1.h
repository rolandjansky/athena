/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BinnedLayerMaterial_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BINNEDLAYERMATERIAL_P1_TRK_H
#define BINNEDLAYERMATERIAL_P1_TRK_H

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"

namespace Trk
{
    /** @class BinnedLayerMaterial_p1 
        */
    class BinnedLayerMaterial_p1
    {
      public:
        BinnedLayerMaterial_p1() :
          splitFactor(0.)
        {}
        
        std::vector<TPObjRef>                   materialMatrix;
        BinUtility_p1                           binUtility;
        float                                   splitFactor;

    };
}

#endif // BINNEDLAYERMATERIAL_P1_TRK_H
