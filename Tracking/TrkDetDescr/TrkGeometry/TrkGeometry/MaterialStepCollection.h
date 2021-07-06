/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepCollection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIALSTEPCOLLECTION_H
#define TRKGEOMETRY_MATERIALSTEPCOLLECTION_H

#include "TrkGeometry/MaterialStep.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
 
namespace Trk {
    typedef DataVector<Trk::MaterialStep> MaterialStepCollection;
}

CLASS_DEF( Trk::MaterialStepCollection , 1274681143 , 1 )

#endif 

