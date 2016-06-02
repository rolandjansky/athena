/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLECONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLECONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrackParticleContainer,
                               xAOD::TrackParticleContainer >
   xAODTrackParticleContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackParticleContainer class
 *
 *         Simple converter class making the xAOD::TrackParticleContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 */
class xAODTrackParticleContainerCnv : public xAODTrackParticleContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackParticleContainerCnv >;

public:
   /// Converter constructor
   xAODTrackParticleContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::TrackParticleContainer* createPersistent( xAOD::TrackParticleContainer* trans );

   /// Function reading in the persistent object
   virtual xAOD::TrackParticleContainer* createTransient();

private:

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrackParticleContainerCnv

#endif 
