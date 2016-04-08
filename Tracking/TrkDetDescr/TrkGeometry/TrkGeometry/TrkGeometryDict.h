/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKGEOMETRY_TRKGEOMETRYDICT_H
#define TRKGEOMETRY_TRKGEOMETRYDICT_H

#include "TrkGeometry/MaterialStep.h"
#include "TrkGeometry/Layer.h"
#include "DataModel/DataVector.h"

namespace TrkDetDetDescrPoolRegister {
     /** the standard vector of MaterialStep */
     std::vector<Trk::MaterialStep*>                            registerMaterialStepVector;
     /** the DataVector of MaterialStep */
     DataVector<Trk::MaterialStep>                              registerMaterialStepDataVector;    
}

#endif
