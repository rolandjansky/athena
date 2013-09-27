/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrtHitCountsCollection_tlp1
 *
 * @brief "top-level" persistent partner for TrigTrtHitCountsCollection
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_TLP1_H

#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p1.h"

class TrigTrtHitCountsCollection_tlp1 
{
   
 public:
   
  TrigTrtHitCountsCollection_tlp1(){}
 
  std::vector<TrigTrtHitCountsCollection_p1>       m_trigTrtHitCountsCollectionVec;
  std::vector<TrigTrtHitCounts_p1>                m_trigTrtHitCountsVec;
  
};//end of class definitions
 
#endif
