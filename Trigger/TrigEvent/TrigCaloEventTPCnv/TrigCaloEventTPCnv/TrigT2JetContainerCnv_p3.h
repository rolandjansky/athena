/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetContainerCnv_p3
 *
 * @brief transient persistent converter for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @modified Tobias Kruker  <kruker@cern.ch>		  		 - U. Bern
 *
 * File and Version Information:
 * $Id: TrigT2JetContainerCnv_p3.h 561410 2013-09-12 14:55:43Z fwinkl $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_CNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p3.h"
 
typedef T_AthenaPoolTPCnvVector< TrigT2JetContainer, TrigT2JetContainer_p3, TrigT2JetCnv_p3 > TrigT2JetContainerCnv_p3;

#endif
