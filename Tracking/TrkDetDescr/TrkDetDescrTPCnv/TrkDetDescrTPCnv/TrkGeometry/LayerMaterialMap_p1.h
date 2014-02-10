/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialMap_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALMAP_P1_TRK_H
#define LAYERMATERIALMAP_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Trk
{
   class LayerMaterialMap_p1 
   {
     public:
       std::vector<TPObjRef> lmVector;
       std::vector<int>      idxVector;
         
       LayerMaterialMap_p1() 
       {}

   };
}

#endif // LAYERMATERIALCOLLECTION_P1_TRK_H