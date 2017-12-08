/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONSATHENAPOOL_XAODTRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINERCNV_H
#define XAODASSOCIATIONSATHENAPOOL_XAODTRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#define private public
#include "xAODAssociations/TrackParticleClusterAssociationAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrackParticleClusterAssociationAuxContainer,
                               xAOD::TrackParticleClusterAssociationAuxContainer >
   xAODTrackParticleClusterAssociationAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackParticleClusterAssociationAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackParticleClusterAssociationContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Heberth Torres <htorres@cern.ch>
 *
 * $Revision: 636929 $
 * $Date: 2014-12-19 00:58:22 +0100 (Fri, 19 Dec 2014) $
 */
class xAODTrackParticleClusterAssociationAuxContainerCnv :
   public xAODTrackParticleClusterAssociationAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackParticleClusterAssociationAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrackParticleClusterAssociationAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrackParticleClusterAssociationAuxContainer*
   createPersistent( xAOD::TrackParticleClusterAssociationAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrackParticleClusterAssociationAuxContainer* createTransient();

}; // class xAODTrackParticleClusterAssociationAuxContainerCnv

#endif // XAODASSOCIATIONSATHENAPOOL_XAODTRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINERCNV_H
