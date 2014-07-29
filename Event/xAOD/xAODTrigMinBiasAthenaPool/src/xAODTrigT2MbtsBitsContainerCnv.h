// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2MbtsBitsContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigT2MbtsBitsCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigT2MbtsBitsCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigT2MbtsBitsContainer,
                               xAOD::TrigT2MbtsBitsContainer >
   xAODTrigT2MbtsBitsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigT2MbtsBitsContainer class
 *
 *         Simple converter class making the xAOD::TrigT2MbtsBitsContainer
 *         class known to POOL.
 *
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigT2MbtsBitsContainerCnv : public xAODTrigT2MbtsBitsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigT2MbtsBitsContainerCnv >;

public:
   /// Converter constructor
   xAODTrigT2MbtsBitsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigT2MbtsBitsContainer*
   createPersistent( xAOD::TrigT2MbtsBitsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigT2MbtsBitsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigT2MbtsBits* cluster ) const;

   /// StoreGate key of the container just being created
    std::string m_key;
    
}; // class xAODTrigT2MbtsBitsContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigT2MbtsBitsCONTAINERCNV_H
