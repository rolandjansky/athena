// Dear emacs, this is -*- c++ -*-
// $Id: xAODTrackCaloClusterAuxContainerCnv.h $
#ifndef XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H
#define XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODPFlow/TrackCaloClusterAuxContainer.h"
#include "xAODTrackCaloClusterAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TrackCaloClusterAuxContainer,
				     xAODTrackCaloClusterAuxContainerCnv_v1 >
   xAODTrackCaloClusterAuxContainerCnv;

/**
 *  @short POOL converter for the xAOD::TrackCaloClusterAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackCaloClusterContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Noemi Calace <Noemi.Calace@cern.ch>
 */

#endif // XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H
