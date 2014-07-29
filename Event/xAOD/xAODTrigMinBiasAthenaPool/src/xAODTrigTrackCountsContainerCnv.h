// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigTrackCountsContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigTrackCountsCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigTrackCountsCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCounts.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigTrackCountsContainer,
                               xAOD::TrigTrackCountsContainer >
   xAODTrigTrackCountsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigTrackCountsContainer class
 *
 *         Simple converter class making the xAOD::TrigTrackCountsContainer
 *         class known to POOL.
 *
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigTrackCountsContainerCnv : public xAODTrigTrackCountsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigTrackCountsContainerCnv >;

public:
   /// Converter constructor
   xAODTrigTrackCountsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigTrackCountsContainer*
   createPersistent( xAOD::TrigTrackCountsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigTrackCountsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigTrackCounts* cluster ) const;

   /// StoreGate key of the container just being created
    std::string m_key;
    
}; // class xAODTrigTrackCountsContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigTrackCountsCONTAINERCNV_H
