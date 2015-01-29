/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCollectionCnv_p4
 *
 * @brief transient-persistent converter for TrigSpacePointCountsCollection_p4
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCollectionCnv_p4.h 563349 2013-09-27 13:20:37Z masik $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_CNV_P4_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_CNV_P4_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p4.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p4.h"

typedef T_AthenaPoolTPCnvVector< TrigSpacePointCountsCollection, TrigSpacePointCountsCollection_p4, TrigSpacePointCountsCnv_p4 > TrigSpacePointCountsCollectionCnv_p4;

 
#endif
