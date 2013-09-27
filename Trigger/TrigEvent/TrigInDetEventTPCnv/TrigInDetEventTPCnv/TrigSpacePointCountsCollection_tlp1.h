/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCollection_tlp1
 *
 * @brief "top-level" persistent partner for TrigSpacePointCountsCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCollection_tlp1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_TLP1_H

#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p3.h"

class TrigSpacePointCountsCollection_tlp1 
{
   
 public:
   
  TrigSpacePointCountsCollection_tlp1(){}
 
  std::vector<TrigSpacePointCountsCollection_p1>       m_trigSpacePointCountsCollectionVec;
  std::vector<TrigSpacePointCounts_p1>                 m_trigSpacePointCountsVec;
  std::vector<TrigSpacePointCounts_p2>                 m_trigSpacePointCountsVec_p2;
  std::vector<TrigSpacePointCounts_p3>                 m_trigSpacePointCountsVec_p3;
     
};//end of class definitions
 
#endif
