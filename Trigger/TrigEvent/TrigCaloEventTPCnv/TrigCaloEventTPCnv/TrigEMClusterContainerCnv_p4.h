/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterContainerCnv_p4
 *
 * @brief transient persistent converter for TrigEMClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterContainerCnv_p4.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_P4_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_P4_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p4.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p4.h"

typedef T_AthenaPoolTPCnvVector<TrigEMClusterContainer,TrigEMClusterContainer_p4, TrigEMClusterCnv_p4> TrigEMClusterContainerCnv_p4;

#endif
