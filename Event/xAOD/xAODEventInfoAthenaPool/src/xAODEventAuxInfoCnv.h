// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTINFOATHENAPOOL_XAODEVENTAUXINFOCNV_H
#define XAODEVENTINFOATHENAPOOL_XAODEVENTAUXINFOCNV_H

// Local include(s).
#include "xAODEventAuxInfoCnv_v2.h"

// EDM include(s).
#include "xAODEventInfo/EventAuxInfo.h"

// Framework include(s).
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// Declare the POOL converter.
typedef T_AthenaPoolAuxContainerCnv< xAOD::EventAuxInfo,
                                     xAODEventAuxInfoCnv_v2 >
   xAODEventAuxInfoCnv;

#endif // XAODEVENTINFOATHENAPOOL_XAODEVENTAUXINFOCNV_H
