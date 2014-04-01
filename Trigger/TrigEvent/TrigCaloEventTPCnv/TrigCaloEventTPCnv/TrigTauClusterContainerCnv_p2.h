/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainerCnv_p2
 *
 * @brief transient persistent converter for TrigTauClusterContainer
 *
  * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p2.h"
 
//class TrigTauClusterContainerCnv_p2 : public T_AthenaPoolTPPtrVectorCnv< TrigTauClusterContainer, TrigTauClusterContainer_p2, TrigTauClusterCnv_p2 >
class TrigTauClusterContainerCnv_p2 : public T_AthenaPoolTPPtrVectorCnv< TrigTauClusterContainer, TrigTauClusterContainer_p2, ITPConverterFor<TrigTauCluster> >
{
 
 public:
  TrigTauClusterContainerCnv_p2() {}
 
}; //end of class definitions
 
#endif
