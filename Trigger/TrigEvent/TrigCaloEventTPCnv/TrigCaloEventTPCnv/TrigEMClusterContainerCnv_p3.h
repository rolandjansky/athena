/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterContainerCnv_p3
 *
 * @brief transient persistent converter for TrigEMClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterContainerCnv_p3.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p3.h"

typedef T_AthenaPoolTPCnvVector<TrigEMClusterContainer,TrigEMClusterContainer_p3, TrigEMClusterCnv_p3> TrigEMClusterContainerCnv_p3;

#endif
