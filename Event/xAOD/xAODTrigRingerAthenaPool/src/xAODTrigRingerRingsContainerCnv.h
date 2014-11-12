// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingerRingsContainerCnv.h
#ifndef XAODRINGERSATHENAPOOL_XAODTRIGRINGERRINGSCONTAINERCNV_H
#define XAODRINGERRATHENAPOOL_XAODTRIGRINGERRINGSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigRingerRingsContainer, xAOD::TrigRingerRingsContainer > xAODTrigRingerRingsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::RingerRingsContainer class
 *
 *         Simple converter class making the xAOD::TrigRingerRingsContainer
 *         class known to POOL.
 *
 * @author Joao Victor da Fonseca Pinto <joao.victor.da.fonseca.pinto@cern.ch>
 * $Date: $
 */
class xAODTrigRingerRingsContainerCnv : public xAODTrigRingerRingsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigRingerRingsContainerCnv >;

public:
   /// Converter constructor
   xAODTrigRingerRingsContainerCnv( ISvcLocator* svcLoc );

   // Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigRingerRingsContainer* createPersistent( xAOD::TrigRingerRingsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigRingerRingsContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrigRingerRingsContainerCnv

#endif // XAOD
