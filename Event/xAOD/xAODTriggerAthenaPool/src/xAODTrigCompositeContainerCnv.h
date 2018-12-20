// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file xAODTriggerAthenaPool/xAODTrigCompositeContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2018
 * @brief 
 */


#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITECONTAINER_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITECONTAINER_H


#include "xAODTrigger/TrigCompositeContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<xAOD::TrigCompositeContainer> xAODTrigCompositeContainerCnv;


#endif // not XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITECONTAINER_H
