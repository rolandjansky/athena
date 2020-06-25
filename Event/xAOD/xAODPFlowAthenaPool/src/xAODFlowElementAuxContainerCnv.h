//  this file is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODPFLOWATHENAPOOL_XAODFLOWELEMENTAUXCONTAINERCNV_H
#define XAODPFLOWATHENAPOOL_XAODFLOWELEMENTAUXCONTAINERCNV_H

// Local include(s).
//#include "xAODFlowEmentAuxContainerCnv_v1.h"

// EDM include(s).
#include "xAODPFlow/FlowElementAuxContainer.h"

// Framework include(s).
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// Declare the POOL converter.
typedef T_AthenaPoolAuxContainerCnv< xAOD::FlowElementAuxContainer>   xAODFlowElementAuxContainerCnv;

#endif // XAODPFLOWATHENAPOOL_XAODFLOWELEMENTAUXCONTAINERCNV_H
