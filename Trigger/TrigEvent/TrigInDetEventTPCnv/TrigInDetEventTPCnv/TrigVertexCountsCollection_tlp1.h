/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCountsCollection_tlp1
 *
 * @brief "top-level" persistent partner for TrigVertexCountsCollection
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCOLLECTION_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCOLLECTION_TLP1_H

#include "TrigInDetEventTPCnv/TrigVertexCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCounts_p1.h"

class TrigVertexCountsCollection_tlp1 
{
   
 public:
   
  TrigVertexCountsCollection_tlp1(){}
 
  std::vector<TrigVertexCountsCollection_p1>      m_trigVertexCountsCollectionVec;
  std::vector<TrigVertexCounts_p1>                m_trigVertexCountsVec;
  
};//end of class definitions
 
#endif
