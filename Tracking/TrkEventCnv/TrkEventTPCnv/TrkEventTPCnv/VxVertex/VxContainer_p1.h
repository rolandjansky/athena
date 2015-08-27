/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXCONTAINER_P1_TRK_H
#define VXCONTAINER_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   VxContainer_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Trk
{
   class VxContainer_p1
      : public std::vector<TPObjRef>
   {
  public:
     VxContainer_p1() {}
   };
}

#endif // VXCONTAINER_P1_TRK_H
