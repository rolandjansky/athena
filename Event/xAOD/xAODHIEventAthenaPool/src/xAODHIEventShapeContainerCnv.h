// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeContainerCnv.h 693514 2015-09-07 15:51:04Z krasznaa $
#ifndef XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_H
#define XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEventShapeContainerCnv_v1.h"


/// Type definition for the converter
typedef T_AthenaPoolxAODCnv< xAOD::HIEventShapeContainer,
                             xAODHIEventShapeContainerCnv_v1 >
   xAODHIEventShapeContainerCnv;


#endif // XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_H
