// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysContainerCnv.h
#ifndef xAODTrigBphysATHENAPOOL_XAODTrigBphysCONTAINERCNV_H
#define xAODTrigBphysATHENAPOOL_XAODTrigBphysCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphys.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigBphysContainer,
                               xAOD::TrigBphysContainer >
   xAODTrigBphysContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigBphysContainer class
 *
 *         Simple converter class making the xAOD::TrigBphysContainer
 *         class known to POOL.
 *
 * @author Your Name Here
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigBphysContainerCnv : public xAODTrigBphysContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigBphysContainerCnv >;

public:
   /// Converter constructor
   xAODTrigBphysContainerCnv( ISvcLocator* svcLoc );

    /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

    
   /// Function preparing the container to be written out
   virtual xAOD::TrigBphysContainer*
   createPersistent( xAOD::TrigBphysContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigBphysContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigBphys* cluster ) const;

    /// StoreGate key of the container just being created
    std::string m_key;

}; // class xAODTrigBphysContainerCnv

#endif // xAODTrigBphysATHENAPOOL_XAODTrigBphysCONTAINERCNV_H
