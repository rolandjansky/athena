// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingerRNNOutputContainerCnv.h
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRNNOutputContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigRNNOutputContainer, xAOD::TrigRNNOutputContainer > xAODTrigRNNOutputContainerCnvBase;
/**
 *  @short POOL converter for the xAOD::TrigRNNOutputContainer class
 *
 *         Simple converter class making the xAOD::TrigRNNOutputContainer
 *         class known to POOL.
 *
 * @author Joao Victor da Fonseca Pinto <joao.victor.da.fonseca.pinto@cern.ch>
 * $Date: $
 */
class xAODTrigRNNOutputContainerCnv : public xAODTrigRNNOutputContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigRNNOutputContainerCnv >;

public:
   /// Converter constructor
   xAODTrigRNNOutputContainerCnv( ISvcLocator* svcLoc );

   // Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigRNNOutputContainer* createPersistent( xAOD::TrigRNNOutputContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigRNNOutputContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODRingerRingsContainerCnv

#endif // XAOD
