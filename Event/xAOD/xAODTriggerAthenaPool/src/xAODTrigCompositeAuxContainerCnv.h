// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file xAODTriggerAthenaPool/xAODTrigCompositeAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2018
 * @brief 
 */


#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H

#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::TrigCompositeAuxContainer>
  xAODTrigCompositeAuxContainerCnv;

#endif // not XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H
