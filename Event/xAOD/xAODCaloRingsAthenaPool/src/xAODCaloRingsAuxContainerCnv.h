/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloRingsAuxContainerCnv.h 659544 2015-04-08 17:39:38Z wsfreund $
#ifndef XAODCALORINGERATHENAPOOL_XAODCALORINGSAUXCONTAINERCNV_H
#define XAODCALORINGERATHENAPOOL_XAODCALORINGSAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloRings/CaloRingsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CaloRingsAuxContainer,
                               xAOD::CaloRingsAuxContainer >
   xAODCaloRingsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloRingsAuxContainer class
 *
 *         This is the first xAOD POOL converter to implement real schema
 *         evolution.
 *
 * @author Werner Freund <wsfreund@cern.ch>
 *
 * based on xAODEgammaAthenaPool
 *
 */
class xAODCaloRingsAuxContainerCnv : public xAODCaloRingsAuxContainerCnvBase {

   /// Declare the factory as our friend
   friend class CnvFactory< xAODCaloRingsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCaloRingsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CaloRingsAuxContainer*
   createPersistent( xAOD::CaloRingsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CaloRingsAuxContainer* createTransient();

}; // class xAODCaloRingsAuxContainerCnv

#endif // XAODCALORINGERATHENAPOOL_XAODCALORINGSAUXCONTAINERCNV_H
