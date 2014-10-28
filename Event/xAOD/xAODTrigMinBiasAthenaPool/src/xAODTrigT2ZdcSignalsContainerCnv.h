// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2ZdcSignalsContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigT2ZdcSignalsCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigT2ZdcSignalsCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigT2ZdcSignalsContainer.h"
#include "xAODTrigMinBias/TrigT2ZdcSignals.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigT2ZdcSignalsContainer,
                               xAOD::TrigT2ZdcSignalsContainer >
   xAODTrigT2ZdcSignalsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigT2ZdcSignalsContainer class
 *
 *         Simple converter class making the xAOD::TrigT2ZdcSignalsContainer
 *         class known to POOL.
 *
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigT2ZdcSignalsContainerCnv : public xAODTrigT2ZdcSignalsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigT2ZdcSignalsContainerCnv >;

public:
   /// Converter constructor
   xAODTrigT2ZdcSignalsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigT2ZdcSignalsContainer*
   createPersistent( xAOD::TrigT2ZdcSignalsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigT2ZdcSignalsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigT2ZdcSignals* cluster ) const;

   /// StoreGate key of the container just being created
    std::string m_key;
    
}; // class xAODTrigT2ZdcSignalsContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigT2ZdcSignalsCONTAINERCNV_H
