/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCountsCollection_p1
 *
 * @brief persistent partner for TrigVertexCountsCollection
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCOLLECTION_P1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCOLLECTION_P1_H


#include "AthenaPoolUtilities/TPObjRef.h"  
#include <vector>
  
class TrigVertexCountsCollection_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigVertexCountsCollection_p1(){}
  friend class TrigVertexCountsCollectionCnv_p1;

 private:

  std::vector<TPObjRef> m_trigVertexCounts;     

};// end of class definitions
 
 
#endif
