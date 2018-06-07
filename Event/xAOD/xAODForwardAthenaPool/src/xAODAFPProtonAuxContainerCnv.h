/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPProtonAuxContainerCnv.h
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Header file with declaration of class xAODAFPProtonAuxContainerCnv. Based on xAODAFPSiHitsClusterAuxContainerCnv.h
 * 
 */


#ifndef AFPPROTONAUXCONTAINERCNV_H
#define AFPPROTONAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPProtonAuxContainer.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPProtonAuxContainer, xAOD::AFPProtonAuxContainer> xAODAFPProtonAuxContainerCnvBase;


class xAODAFPProtonAuxContainerCnv :
  public xAODAFPProtonAuxContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPProtonAuxContainerCnv >;
 
protected:
  /// Converter constructor
  xAODAFPProtonAuxContainerCnv( ISvcLocator* svcLoc );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPProtonAuxContainer*
  createPersistent( xAOD::AFPProtonAuxContainer* trans );
  
  /// Function reading in the object from the input file
  virtual xAOD::AFPProtonAuxContainer* createTransient();
 
};

#endif
