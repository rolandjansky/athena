/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStepCollection_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIALSTEPCOLLECTION_P1_TRK_H
#define MATERIALSTEPCOLLECTION_P1_TRK_H

#include "MaterialStep_p1.h"
#include <vector>

namespace Trk
{
   class MaterialStepCollection_p1 : public std::vector<Trk::MaterialStep_p1>
   {
     public:
       MaterialStepCollection_p1() {}
   };
}

#endif // MATERIALSTEPCOLLECTION_P1_TRK_H