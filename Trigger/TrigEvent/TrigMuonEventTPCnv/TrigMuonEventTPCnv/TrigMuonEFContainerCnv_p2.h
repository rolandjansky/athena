/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFContainerCnv_p2
 *
 * @brief transient-persistent converter for TrigMuonContainer_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEFContainerCnv_p2.h 561479 2013-09-13 10:06:03Z fwinkl $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_CNV_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p2.h"

typedef T_AthenaPoolTPCnvVector< TrigMuonEFContainer, TrigMuonEFContainer_p2, TrigMuonEFCnv_p2 > TrigMuonEFContainerCnv_p2;
 
#endif
