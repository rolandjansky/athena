/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainerCnv_p1
 *
 * @brief transient persistent converter for TrigTauClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauClusterContainerCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p1.h"
 
//class TrigTauClusterContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigTauClusterContainer, TrigTauClusterContainer_p1, TrigTauClusterCnv_p1 >
class TrigTauClusterContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigTauClusterContainer, TrigTauClusterContainer_p1, ITPConverterFor<TrigTauCluster> >
{
 
 public:
  TrigTauClusterContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
