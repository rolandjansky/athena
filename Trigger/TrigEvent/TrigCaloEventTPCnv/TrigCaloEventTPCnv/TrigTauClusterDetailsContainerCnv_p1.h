/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsContainerCnv_p1
 *
 * @brief transient persistent converter for TrigTauClusterDetailsContainer
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p1.h"
 
class TrigTauClusterDetailsContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigTauClusterDetailsContainer, TrigTauClusterDetailsContainer_p1, TrigTauClusterDetailsCnv_p1 >
{
 
 public:
  TrigTauClusterDetailsContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
