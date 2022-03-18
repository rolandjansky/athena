/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementsLayerVectors_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementsLayerVectors_xk_H
#define SiDetElementsLayerVectors_xk_H

#include "SiDetElementsRoadTool_xk/SiDetElementsLayer_xk.h"
#include <vector>

/**
 * SiDetElementsLayerVectors_xk is a vector of SiDetElementsLayer_xk.
 * SiDetElementsLayerVectors_xk is prepared by SiDetElementsRoadCondAlg_xk,
 * stored in condition store and is used by SiDetElementsRoadMaker_xk.
 */

namespace InDet {
  typedef std::vector<std::vector<InDet::SiDetElementsLayer_xk>> SiDetElementsLayerVectors_xk;
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDet::SiDetElementsLayerVectors_xk , 231384279 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_MIXED_DEF( InDet::SiDetElementsLayerVectors_xk , 226495641 );

#endif // SiDetElementsLayerVectors_xk_H
