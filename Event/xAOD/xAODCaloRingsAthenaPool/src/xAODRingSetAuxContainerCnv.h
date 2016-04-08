/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetAuxContainerCnv.h 659544 2015-04-08 17:39:38Z wsfreund $
#ifndef XAODCALORINGERATHENAPOOL_XAODRINGSETAUXCONTAINERCNV_H
#define XAODCALORINGERATHENAPOOL_XAODRINGSETAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloRings/RingSetAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::RingSetAuxContainer,
                               xAOD::RingSetAuxContainer >
   xAODRingSetAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::RingSetAuxContainer class
 *
 *         This is the first xAOD POOL converter to implement real schema
 *         evolution.
 *
 * @author Werner Freund <wsfreund@cern.ch>
 *
 * based on xAODEgammaAthenaPool
 *
 */
class xAODRingSetAuxContainerCnv : public xAODRingSetAuxContainerCnvBase {

   /// Declare the factory as our friend
   friend class CnvFactory< xAODRingSetAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODRingSetAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::RingSetAuxContainer*
   createPersistent( xAOD::RingSetAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::RingSetAuxContainer* createTransient();

}; // class xAODRingSetAuxContainerCnv

#endif // XAODCALORINGERATHENAPOOL_XAODRINGSETAUXCONTAINERCNV_H
