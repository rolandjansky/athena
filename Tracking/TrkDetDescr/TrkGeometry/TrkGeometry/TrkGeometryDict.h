/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKGEOMETRY_TRKGEOMETRYDICT_H
#define TRKGEOMETRY_TRKGEOMETRYDICT_H

#include "TrkGeometry/MaterialStepVector.h"
#include "TrkGeometry/MaterialStep.h"
#include "DataModel/DataVector.h"

namespace TrkDetDetDescrPoolRegister {
     /** the standard vector of MaterialStep */
     std::vector<Trk::MaterialStep*>                            m_registerMaterialStepVector;
     /** the DataVector of MaterialStep */
     DataVector<Trk::MaterialStep>                              m_registerMaterialStepDataVector;    
}

#endif
