/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrackCountsCollectionCnv_p1
 *
 * @brief transient-persistent converter for TrigTrackCountsCollection_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTrackCountsCollectionCnv_p1.h,v 1.3 2009-04-24 22:08:13 wbell Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCONTAINER_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigTrackCountsCollection.h"
#include "TrigInDetEvent/TrigTrackCounts.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p1.h"

class TrigTrackCountsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigTrackCountsCollection, TrigTrackCountsCollection_p1, ITPConverterFor<TrigTrackCounts> >
{
 
 public:
  TrigTrackCountsCollectionCnv_p1() {}

}; //end of class definitions
 
#endif
