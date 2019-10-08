// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file xAODTriggerAthenaPool/xAODTrigCompositeAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2018
 * @brief 
 */


#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H

// stystem includes
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigCompositeAuxContainerCnv_v1.h"


/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TrigCompositeAuxContainer,
                                     xAODTrigCompositeAuxContainerCnv_v1 >
   xAODTrigCompositeAuxContainerCnv;


#endif // not XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H
