/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPSiHitsClusterContainerCnv.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-18
 * 
 * @brief  Declaration of xAODAFPSiHitsClusterContainerCnv class.
 * 
 */


#ifndef XAODAFPSIHITSCLUSTERCONTAINERCNV_H
#define XAODAFPSIHITSCLUSTERCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsCluster.h"


typedef T_AthenaPoolCustomCnv<xAOD::AFPSiHitsClusterContainer, xAOD::AFPSiHitsClusterContainer> xAODAFPSiHitsClusterContainerCnvBase;

class xAODAFPSiHitsClusterContainerCnv : public xAODAFPSiHitsClusterContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPSiHitsClusterContainerCnv >;
 
public:
  /// Converter constructor
  xAODAFPSiHitsClusterContainerCnv( ISvcLocator* svcLoc );
 
  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPSiHitsClusterContainer* createPersistent( xAOD::AFPSiHitsClusterContainer* trans );
  
  /// Function reading in the persistent object
  virtual xAOD::AFPSiHitsClusterContainer* createTransient();
 
private:
  /// Function preparing a cluster object for persistence
  void toPersistent( xAOD::AFPSiHitsCluster* cluster ) const;
 
  /// StoreGate key of the container just being created
  std::string m_key;
};


#endif	// XAODAFPSIHITSCLUSTERCONTAINERCNV_H

