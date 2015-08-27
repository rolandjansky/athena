/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXCONTAINER_P1_TRK_H
#define MVFVXCONTAINER_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   VxContainer_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Trk
{
   class MVFVxContainer_p1 : public std::vector<TPObjRef>
   {
  public:
     MVFVxContainer_p1() {}
   };
}

#endif // MVFVXCONTAINER_P1_TRK_H
