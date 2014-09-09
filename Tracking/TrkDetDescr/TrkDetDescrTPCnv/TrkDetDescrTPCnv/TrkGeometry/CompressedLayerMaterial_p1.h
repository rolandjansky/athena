/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// CompressedLayerMaterial_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPRESSEDLAYERMATERIAL_P1_TRK_H
#define COMPRESSEDLAYERMATERIAL_P1_TRK_H

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"

namespace Trk
{
    /** @class CompressedLayerMaterial_p1 
        */
    class CompressedLayerMaterial_p1
    {
      public:
        CompressedLayerMaterial_p1() :
         splitFactor(1.)
        {}
        
         std::vector<TPObjRef>              materialVector;
         std::vector<unsigned short int >   materialBins;
         BinUtility_p1                      binUtility;
         float                              splitFactor;

    };
}

#endif // COMPRESSEDLAYERMATERIAL_P1_TRK_H