// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigSpacePointCountsContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigSpacePointCountsCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigSpacePointCountsCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCounts.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigSpacePointCountsContainer,
                               xAOD::TrigSpacePointCountsContainer >
   xAODTrigSpacePointCountsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigSpacePointCountsContainer class
 *
 *         Simple converter class making the xAOD::TrigSpacePointCountsContainer
 *         class known to POOL.
 *
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigSpacePointCountsContainerCnv : public xAODTrigSpacePointCountsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigSpacePointCountsContainerCnv >;

public:
   /// Converter constructor
   xAODTrigSpacePointCountsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigSpacePointCountsContainer*
   createPersistent( xAOD::TrigSpacePointCountsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigSpacePointCountsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigSpacePointCounts* cluster ) const;

   /// StoreGate key of the container just being created
    std::string m_key;
    
}; // class xAODTrigSpacePointCountsContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigSpacePointCountsCONTAINERCNV_H
