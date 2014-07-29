// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigVertexCountsContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigVertexCountsCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigVertexCountsCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigVertexCountsContainer.h"
#include "xAODTrigMinBias/TrigVertexCounts.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigVertexCountsContainer,
                               xAOD::TrigVertexCountsContainer >
   xAODTrigVertexCountsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigVertexCountsContainer class
 *
 *         Simple converter class making the xAOD::TrigVertexCountsContainer
 *         class known to POOL.
 *
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigVertexCountsContainerCnv : public xAODTrigVertexCountsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigVertexCountsContainerCnv >;

public:
   /// Converter constructor
   xAODTrigVertexCountsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigVertexCountsContainer*
   createPersistent( xAOD::TrigVertexCountsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigVertexCountsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigVertexCounts* cluster ) const;

   /// StoreGate key of the container just being created
    std::string m_key;
    
}; // class xAODTrigVertexCountsContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigVertexCountsCONTAINERCNV_H
