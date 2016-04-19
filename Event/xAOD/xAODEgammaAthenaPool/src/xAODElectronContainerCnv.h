// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronContainerCnv.h 671839 2015-06-02 19:44:47Z christos $
#ifndef XAODELECTRONATHENAPOOL_XAODELECTRONCONTAINERCNV_H
#define XAODELECTRONATHENAPOOL_XAODELECTRONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"

// System include(s): 
#include <string> 

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::ElectronContainer,
                               xAOD::ElectronContainer >
   xAODElectronContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::ElectronContainer class
 *
 *         Simple converter class making the xAOD::ElectronContainer
 *         class known to POOL.
 *
 * @author Christos Anastopoulos
 *
 * $Revision: 671839 $
 * $Date: 2015-06-02 21:44:47 +0200 (Tue, 02 Jun 2015) $
 */
class xAODElectronContainerCnv : public xAODElectronContainerCnvBase {
  
  // Declare the factory as our friend:
  friend class CnvFactory< xAODElectronContainerCnv >;

public:
  /// Converter constructor
  xAODElectronContainerCnv( ISvcLocator* svcLoc );

  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ); 
  /// Function preparing the container to be written out
  virtual xAOD::ElectronContainer* createPersistent( xAOD::ElectronContainer* trans );
  /// Function reading in the persistent object
  virtual xAOD::ElectronContainer* createTransient();
  
private:
  /// StoreGate key of the container just being created 
  std::string m_key; 
  
}; // class xAODElectronContainerCnv

#endif // XAODELECTRONATHENAPOOL_XAODELECTRONCONTAINERCNV_H
