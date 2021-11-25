/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file AFPToFTrackContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 */

#ifndef AFPTOFTRACKCONTAINERCNV_H
#define AFPTOFTRACKCONTAINERCNV_H

#include "xAODForward/AFPToFTrackContainer.h"
#include "xAODForward/AFPToFTrack.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPToFTrackContainer, xAOD::AFPToFTrackContainer> xAODAFPToFTrackContainerCnvBase;

class xAODAFPToFTrackContainerCnv : public xAODAFPToFTrackContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPToFTrackContainerCnv >;
 
public:
  /// Converter constructor
  xAODAFPToFTrackContainerCnv( ISvcLocator* svcLoc );
 
  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPToFTrackContainer* createPersistent( xAOD::AFPToFTrackContainer* trans );
  
  /// Function reading in the persistent object
  virtual xAOD::AFPToFTrackContainer* createTransient();
 
private:
  /// Function preparing a vertex object for persistence
  void toPersistent( xAOD::AFPToFTrack* track ) const;
 
  /// StoreGate key of the container just being created
  std::string m_key;
 
};


#endif

