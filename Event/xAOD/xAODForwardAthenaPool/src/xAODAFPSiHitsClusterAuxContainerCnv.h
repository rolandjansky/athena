/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPSiHitsClusterAuxContainerCnv.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-18
 * 
 * @brief  Header file with declaration of class xAODAFPSiHitsClusterAuxContainerCnv.
 * 
 */


#ifndef XAODAFPSIHITSCLUSTERAUXCONTAINERCNV_H
#define XAODAFPSIHITSCLUSTERAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPSiHitsClusterAuxContainer, xAOD::AFPSiHitsClusterAuxContainer> xAODAFPSiHitsClusterAuxContainerCnvBase;


class xAODAFPSiHitsClusterAuxContainerCnv :
  public xAODAFPSiHitsClusterAuxContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPSiHitsClusterAuxContainerCnv >;
 
protected:
  /// Converter constructor
  xAODAFPSiHitsClusterAuxContainerCnv( ISvcLocator* svcLoc );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPSiHitsClusterAuxContainer*
  createPersistent( xAOD::AFPSiHitsClusterAuxContainer* trans );
  
  /// Function reading in the object from the input file
  virtual xAOD::AFPSiHitsClusterAuxContainer* createTransient();
 
};


#endif	//  XAODAFPSIHITSCLUSTERAUXCONTAINERCNV_H

