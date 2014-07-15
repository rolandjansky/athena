// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPhotonContainerCnv.h
#ifndef xAODTrigEgammaATHENAPOOL_XAODTrigPhotonCONTAINERCNV_H
#define xAODTrigEgammaATHENAPOOL_XAODTrigPhotonCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhoton.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigPhotonContainer,
                               xAOD::TrigPhotonContainer >
   xAODTrigPhotonContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigPhotonContainer class
 *
 *         Simple converter class making the xAOD::TrigPhotonContainer
 *         class known to POOL.
 *
 * @author Your Name Here
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigPhotonContainerCnv : public xAODTrigPhotonContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigPhotonContainerCnv >;

public:
   /// Converter constructor
   xAODTrigPhotonContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ); 
   /// Function preparing the container to be written out
   virtual xAOD::TrigPhotonContainer*
   createPersistent( xAOD::TrigPhotonContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigPhotonContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigPhoton* cluster ) const;

   /// StoreGate key for the container just being created
   std::string m_key;

}; // class xAODTrigPhotonContainerCnv

#endif // xAODTrigEgammaATHENAPOOL_XAODTrigPhotonCONTAINERCNV_H
