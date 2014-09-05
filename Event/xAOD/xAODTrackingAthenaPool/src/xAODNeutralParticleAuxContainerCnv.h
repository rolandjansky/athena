/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODNEUTRALPARTICLEAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODNEUTRALPARTICLEAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/NeutralParticleAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::NeutralParticleAuxContainer,
                               xAOD::NeutralParticleAuxContainer >
   xAODNeutralParticleAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::NeutralParticleAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackParticleContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 */
class xAODNeutralParticleAuxContainerCnv :
   public xAODNeutralParticleAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODNeutralParticleAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODNeutralParticleAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::NeutralParticleAuxContainer*
   createPersistent( xAOD::NeutralParticleAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::NeutralParticleAuxContainer* createTransient();

}; // class xAODNeutralParticleAuxContainerCnv

#endif 
