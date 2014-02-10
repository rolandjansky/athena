/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialCollection_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALCOLLECTION_P1_TRK_H
#define LAYERMATERIALCOLLECTION_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Trk
{
   class LayerMaterialCollection_p1 : public std::vector<TPObjRef> 
   {
     public:
       LayerMaterialCollection_p1() 
       {}

   };
}

#endif // LAYERMATERIALCOLLECTION_P1_TRK_H