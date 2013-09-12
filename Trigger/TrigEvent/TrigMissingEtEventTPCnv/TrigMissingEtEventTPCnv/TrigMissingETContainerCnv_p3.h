/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETContainerCnv_p3
 *
 * @brief transient persistent converter for TrigMissingETContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMissingETContainerCnv_p3.h 561414 2013-09-12 15:13:09Z fwinkl $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCONTAINER_CNV_P3_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p3.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p3.h"
 
typedef T_AthenaPoolTPCnvVector< TrigMissingETContainer, TrigMissingETContainer_p3, TrigMissingETCnv_p3 > TrigMissingETContainerCnv_p3;
 
#endif
