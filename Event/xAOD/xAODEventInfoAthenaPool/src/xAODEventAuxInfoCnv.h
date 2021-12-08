// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTINFOATHENAPOOL_XAODEVENTAUXINFOCNV_H
#define XAODEVENTINFOATHENAPOOL_XAODEVENTAUXINFOCNV_H

// Local include(s).
#include "xAODEventAuxInfoCnv_v2.h"
#include "xAODEventAuxInfoCnv_v3.h"

// EDM include(s).
#include "xAODEventInfo/EventAuxInfo.h"

// Framework include(s).
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// Declare the POOL converter.
typedef T_AthenaPoolAuxContainerCnv< xAOD::EventAuxInfo,
                                     xAODEventAuxInfoCnv_v2,
                                     xAODEventAuxInfoCnv_v3 >
   xAODEventAuxInfoCnv;

#endif // XAODEVENTINFOATHENAPOOL_XAODEVENTAUXINFOCNV_H
