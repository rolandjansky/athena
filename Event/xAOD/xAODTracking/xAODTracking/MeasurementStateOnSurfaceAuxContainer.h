/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_MEASUREMENTSTATEONSURFACEAUXCONTAINER_H
#define XAODTRACKING_MEASUREMENTSTATEONSURFACEAUXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/versions/MeasurementStateOnSurfaceAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current MeasurementStateOnSurface auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::MeasurementStateOnSurfaceContainer,
   /// so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef MeasurementStateOnSurfaceAuxContainer_v1 MeasurementStateOnSurfaceAuxContainer;
}
 
#endif // XAODTRACKING_MEASUREMENTSTATEONSURFACEAUXCONTAINER_H
