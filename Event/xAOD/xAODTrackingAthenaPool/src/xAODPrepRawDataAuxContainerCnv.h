/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODPREPRAWDATAAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODPREPRAWDATAAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/PrepRawDataAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::PrepRawDataAuxContainer,
                               xAOD::PrepRawDataAuxContainer >
   xAODPrepRawDataAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::PrepRawDataAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackParticleContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 */
class xAODPrepRawDataAuxContainerCnv :
   public xAODPrepRawDataAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODPrepRawDataAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODPrepRawDataAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::PrepRawDataAuxContainer*
   createPersistent( xAOD::PrepRawDataAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::PrepRawDataAuxContainer* createTransient();

}; // class xAODPrepRawDataAuxContainerCnv

#endif 
