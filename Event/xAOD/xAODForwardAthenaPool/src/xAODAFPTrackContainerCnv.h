/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file AFPTrackContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef AFPTRACKCONTAINERCNV_H
#define AFPTRACKCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPTrackContainer, xAOD::AFPTrackContainer> xAODAFPTrackContainerCnvBase;

class xAODAFPTrackContainerCnv : public xAODAFPTrackContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPTrackContainerCnv >;
 
public:
  /// Converter constructor
  xAODAFPTrackContainerCnv( ISvcLocator* svcLoc );
 
  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPTrackContainer* createPersistent( xAOD::AFPTrackContainer* trans );
  
  /// Function reading in the persistent object
  virtual xAOD::AFPTrackContainer* createTransient();
 
private:
  /// Function preparing a vertex object for persistence
  void toPersistent( xAOD::AFPTrack* track ) const;
 
  /// StoreGate key of the container just being created
  std::string m_key;
 
};


#endif

