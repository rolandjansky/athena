/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutputContainerCnv_p2
 *
 * @brief transient persistent converter for TrigRNNOutputContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id: TrigRNNOutputContainerCnv_p2.h 561410 2013-09-12 14:55:43Z fwinkl $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_CNV_P2_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigRNNOutputContainer.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p2.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_p2.h"
 
typedef T_AthenaPoolTPCnvVector<TrigRNNOutputContainer, TrigRNNOutputContainer_p2, TrigRNNOutputCnv_p2> TrigRNNOutputContainerCnv_p2;

#endif

