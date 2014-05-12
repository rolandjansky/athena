/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerIndexSampleMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_LAYERINDEXSAMPLEMAP_H
#define TRKDETDESCRUTILS_LAYERINDEXSAMPLEMAP_H

#include <map>
// from TrkDetDescrUtils
#include "TrkDetDescrUtils/LayerIndex.h"
// CLASS DEF
#include "CLIDSvc/CLASS_DEF.h"

/** LayerIndexSampleMap

   instead of the CaloCell_ID (enum) a simple int is taken,
   so, implicit conversion is needed.
   
   pos/neg endcap are distinguished by +/-
   
*/

namespace Trk {

 typedef std::map<Trk::LayerIndex,int> LayerIndexSampleMap;

}

// clid LayerIndexSampleMap
// 267485628 LayerIndexSampleMap

CLASS_DEF(Trk::LayerIndexSampleMap, 267485628, 1)

#endif


