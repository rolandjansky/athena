/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKGEOMETRY_TRKGEOMETRYDICT_H
#define TRKGEOMETRY_TRKGEOMETRYDICT_H

#include "AthContainers/DataVector.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialStep.h"

namespace TrkDetDetDescrPoolRegister {
/** the standard vector of MaterialStep */
std::vector<Trk::MaterialStep*> registerMaterialStepVector;
/** the DataVector of MaterialStep */
DataVector<Trk::MaterialStep> registerMaterialStepDataVector;
}  // namespace TrkDetDetDescrPoolRegister

#endif
