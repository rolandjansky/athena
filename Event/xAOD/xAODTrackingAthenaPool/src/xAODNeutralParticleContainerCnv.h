/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODNEUTRALPARTICLECONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODNEUTRALPARTICLECONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/NeutralParticleContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::NeutralParticleContainer,
                               xAOD::NeutralParticleContainer >
   xAODNeutralParticleContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::NeutralParticleContainer class
 *
 *         Simple converter class making the xAOD::NeutralParticleContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 */
class xAODNeutralParticleContainerCnv : public xAODNeutralParticleContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODNeutralParticleContainerCnv >;

public:
   /// Converter constructor
   xAODNeutralParticleContainerCnv( ISvcLocator* svcLoc );
   
   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::NeutralParticleContainer* createPersistent( xAOD::NeutralParticleContainer* trans );
   
   /// Function reading in the persistent object
   virtual xAOD::NeutralParticleContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::NeutralParticle* np ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODNeutralParticleContainerCnv

#endif 
