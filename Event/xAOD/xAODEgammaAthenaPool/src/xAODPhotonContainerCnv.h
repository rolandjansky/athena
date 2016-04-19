// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonContainerCnv.h 671839 2015-06-02 19:44:47Z christos $
#ifndef XAODPHOTONATHENAPOOL_XAODPHOTONCONTAINERCNV_H
#define XAODPHOTONATHENAPOOL_XAODPHOTONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"

// System include(s): 
#include <string> 

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::PhotonContainer,
                               xAOD::PhotonContainer >
xAODPhotonContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::PhotonContainer class
 *
 *         Simple converter class making the xAOD::PhotonContainer
 *         class known to POOL.
 *
 * @author Christos Anastopoulos
 *
 * $Revision: 671839 $
 * $Date: 2015-06-02 21:44:47 +0200 (Tue, 02 Jun 2015) $
 */
class xAODPhotonContainerCnv : public xAODPhotonContainerCnvBase {
  
  // Declare the factory as our friend:
  friend class CnvFactory< xAODPhotonContainerCnv >;

public:
  /// Converter constructor
  xAODPhotonContainerCnv( ISvcLocator* svcLoc );
  
  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ); 
  /// Function preparing the container to be written out
  virtual xAOD::PhotonContainer* createPersistent( xAOD::PhotonContainer* trans );
  /// Function reading in the persistent object
  virtual xAOD::PhotonContainer* createTransient();
  
private:
  /// StoreGate key of the container just being created 
  std::string m_key; 
    
}; // class xAODPhotonContainerCnv

#endif // XAODPHOTONATHENAPOOL_XAODPHOTONCONTAINERCNV_H
