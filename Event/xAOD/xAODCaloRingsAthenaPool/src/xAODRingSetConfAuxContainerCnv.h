/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetConfAuxContainerCnv.h 659544 2015-04-08 17:39:38Z wsfreund $
#ifndef XAODCALORINGERATHENAPOOL_XAODRINGSETCONFAUXCONTAINERCNV_H
#define XAODCALORINGERATHENAPOOL_XAODRINGSETCONFAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloRings/RingSetConfAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::RingSetConfAuxContainer,
                               xAOD::RingSetConfAuxContainer >
   xAODRingSetConfAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::RingSetConfAuxContainer class
 *
 *         This is the first xAOD POOL converter to implement real schema
 *         evolution.
 *
 * @author Werner Freund <wsfreund@cern.ch>
 *
 * based on xAODEgammaAthenaPool
 *
 */
class xAODRingSetConfAuxContainerCnv : public xAODRingSetConfAuxContainerCnvBase {

   /// Declare the factory as our friend
   friend class CnvFactory< xAODRingSetConfAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODRingSetConfAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::RingSetConfAuxContainer*
   createPersistent( xAOD::RingSetConfAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::RingSetConfAuxContainer* createTransient();

}; // class xAODRingSetConfAuxContainerCnv

#endif // XAODCALORINGERATHENAPOOL_XAODRINGSETCONFAUXCONTAINERCNV_H
