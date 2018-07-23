/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialCollection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETY_LAYERMATERIALCOLLECTION_H
#define TRKGEOMETY_LAYERMATERIALCOLLECTION_H

// Trk & Gaudi
#include "TrkGeometry/LayerMaterialProperties.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

namespace Trk {

    typedef DataVector<Trk::LayerMaterialProperties> LayerMaterialCollection;
    
}

CLASS_DEF( Trk::LayerMaterialCollection, 1264846423, 1 )

#endif
