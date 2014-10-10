// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthParticleContainerCnv.h 582241 2014-02-07 17:28:39Z jcatmore $
#ifndef XAODTRUTHATHENAPOOL_XAODTRUTHPARTICLECONTAINERCNV_H
#define XAODTRUTHATHENAPOOL_XAODTRUTHPARTICLECONTAINERCNV_H

// Base package(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM package(s):
#include "xAODTruth/TruthParticleContainer.h"

/// The converter is just the specialisation of the template type
typedef T_AthenaPoolCustomCnv< xAOD::TruthParticleContainer, 
			       xAOD::TruthParticleContainer >
   xAODTruthParticleContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TruthParticleContainer class
 *
 *         Simple converter class making the xAOD::TruthParticleContainer
 *         class known to POOL. (based on xAOD::JetContainer)
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
 *
 * $Revision: 582241 $
 * $Date: 2014-02-07 18:28:39 +0100 (Fri, 07 Feb 2014) $
 */
class xAODTruthParticleContainerCnv : public xAODTruthParticleContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTruthParticleContainerCnv >;

public:
   /// Converter constructor
   xAODTruthParticleContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key 
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TruthParticleContainer* createPersistent( xAOD::TruthParticleContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TruthParticleContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TruthParticle* cluster ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTruthParticleContainerCnv


#endif // XAODTRUTHATHENAPOOL_XAODTRUTHPARTICLECONTAINERCNV_H
