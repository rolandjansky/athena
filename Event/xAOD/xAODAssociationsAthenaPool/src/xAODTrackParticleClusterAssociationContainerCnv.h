/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONSATHENAPOOL_XAODTRACKPARTICLECLUSTERASSOCIATIONCONTAINERCNV_H
#define XAODASSOCIATIONSATHENAPOOL_XAODTRACKPARTICLECLUSTERASSOCIATIONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrackParticleClusterAssociationContainer,
                               xAOD::TrackParticleClusterAssociationContainer >
   xAODTrackParticleClusterAssociationContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackParticleClusterAssociationContainer class
 *
 *         Simple converter class making the xAOD::TrackParticleClusterAssociationContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Heberth Torres <htorres@cern.ch>
 *
 */
class xAODTrackParticleClusterAssociationContainerCnv : public xAODTrackParticleClusterAssociationContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackParticleClusterAssociationContainerCnv >;

public:
   /// Converter constructor
   xAODTrackParticleClusterAssociationContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::TrackParticleClusterAssociationContainer* createPersistent( xAOD::TrackParticleClusterAssociationContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrackParticleClusterAssociationContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrackParticleClusterAssociation* trackParticleClusterAssociation ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrackParticleClusterAssociationContainerCnv

#endif // XAODASSOCIATIONSATHENAPOOL_XAODTRACKPARTICLECLUSTERASSOCIATIONCONTAINERCNV_H
