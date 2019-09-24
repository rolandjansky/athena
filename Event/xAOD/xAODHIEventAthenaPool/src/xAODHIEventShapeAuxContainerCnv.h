// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_H
#define XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODHIEventShapeAuxContainerCnv_v1.h"

// EDM include(s):
#include "xAODHIEvent/HIEventShapeAuxContainer.h"


/// Type definition for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::HIEventShapeAuxContainer,
                                     xAODHIEventShapeAuxContainerCnv_v1 >
   xAODHIEventShapeAuxContainerCnv;


#endif // XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_H
