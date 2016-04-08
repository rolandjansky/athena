// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigMissingETContainerCnv.h 592512 2014-04-11 09:38:00Z krasznaa $
#ifndef xAODTrigMissingETATHENAPOOL_XAODTrigMissingETCONTAINERCNV_H
#define xAODTrigMissingETATHENAPOOL_XAODTrigMissingETCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMissingET/TrigMissingETContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigMissingETContainer,
                               xAOD::TrigMissingETContainer >
   xAODTrigMissingETContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigMissingETContainer class
 *
 *         Simple converter class making the xAOD::TrigMissingETContainer
 *         class known to POOL.
 *
 * @author Your Name Here
 *
 * $Revision: 592512 $
 * $Date: 2014-04-11 11:38:00 +0200 (Fri, 11 Apr 2014) $
 */
class xAODTrigMissingETContainerCnv : public xAODTrigMissingETContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigMissingETContainerCnv >;

public:
   /// Converter constructor
   xAODTrigMissingETContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigMissingETContainer*
   createPersistent( xAOD::TrigMissingETContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigMissingETContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrigMissingETContainerCnv

#endif // xAODTrigMissingETATHENAPOOL_XAODTrigMissingETCONTAINERCNV_H
