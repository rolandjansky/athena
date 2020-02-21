/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _ROIInfoVec_h_
#define _ROIInfoVec_h_

#include <vector>
#include "AthenaKernel/CLASS_DEF.h"

namespace InDet {
   struct ROIInfo {
      ROIInfo (double the_emF,
               double the_emR,
               double the_emZ)
         : emF(the_emF),
           emR(the_emR),
           emZ(the_emZ) {}
      double emF;
      double emR;
      double emZ;
   };
   using ROIInfoVec = std::vector<ROIInfo>;
}

CLASS_DEF (InDet::ROIInfoVec, 165547689, 0)

#endif
