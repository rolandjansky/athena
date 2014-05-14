/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepCollection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIALSTEPCOLLECTION_H
#define TRKGEOMETRY_MATERIALSTEPCOLLECTION_H

#include "TrkGeometry/MaterialStep.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
 
namespace Trk {
    typedef DataVector<Trk::MaterialStep> MaterialStepCollection;
}

CLASS_DEF( Trk::MaterialStepCollection , 1274681143 , 1 )

#endif 

