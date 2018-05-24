/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPProtonContainerCnv.h
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Declaration of xAODAFPProtonContainerCnv class. Based on xAODAFPSiHitsClusterContainerCnv.h
 * 
 */


#ifndef XAODAFPPROTONCONTAINERCNV_H
#define XAODAFPPROTONCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPProtonContainer.h"
#include "xAODForward/AFPProton.h"


typedef T_AthenaPoolCustomCnv<xAOD::AFPProtonContainer, xAOD::AFPProtonContainer> xAODAFPProtonContainerCnvBase;

class xAODAFPProtonContainerCnv : public xAODAFPProtonContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPProtonContainerCnv >;
 
public:
  /// Converter constructor
  xAODAFPProtonContainerCnv( ISvcLocator* svcLoc );
 
  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPProtonContainer* createPersistent( xAOD::AFPProtonContainer* trans );
  
  /// Function reading in the persistent object
  virtual xAOD::AFPProtonContainer* createTransient();
 
private:
  /// Function preparing a proton object for persistence
  void toPersistent( xAOD::AFPProton* proton ) const;
 
  /// StoreGate key of the container just being created
  std::string m_key;
};


#endif	// XAODAFPSIHITSCLUSTERCONTAINERCNV_H

