// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// $Id: xAODTrackCaloClusterAuxContainerCnv.h $
#ifndef XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H
#define XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODPFlow/TrackCaloClusterAuxContainer.h"
#include "xAODTrackCaloClusterAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TrackCaloClusterAuxContainer,
                                     xAODTrackCaloClusterAuxContainerCnv_v1 >
   xAODTrackCaloClusterAuxContainerCnv;


#endif // XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H
