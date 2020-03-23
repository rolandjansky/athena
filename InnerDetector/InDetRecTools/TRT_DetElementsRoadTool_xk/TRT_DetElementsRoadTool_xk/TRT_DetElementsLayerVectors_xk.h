/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_DetElementsLayerVectors_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_DetElementsLayerVectors_xk_H
#define TRT_DetElementsLayerVectors_xk_H

#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsLayer_xk.h"
#include <vector>

namespace InDet {
  typedef std::vector<std::vector<InDet::TRT_DetElementsLayer_xk>> TRT_DetElementsLayerVectors_xk;
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDet::TRT_DetElementsLayerVectors_xk , 48196303 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDet::TRT_DetElementsLayerVectors_xk , 61507347 );

#endif //TRT_DetElementsLayerVectors_xk_H
