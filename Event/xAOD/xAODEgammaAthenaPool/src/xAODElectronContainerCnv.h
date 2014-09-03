// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronContainerCnv.h 581401 2014-02-04 14:12:49Z christos $
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
 * $Revision: 581401 $
 * $Date: 2014-02-04 15:12:49 +0100 (Tue, 04 Feb 2014) $
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
  /// Function preparing a vertex object for persistence
  void toPersistent( xAOD::Electron* cluster ) const;

  /// StoreGate key of the container just being created 
  std::string m_key; 
  
}; // class xAODElectronContainerCnv

#endif // XAODELECTRONATHENAPOOL_XAODELECTRONCONTAINERCNV_H
