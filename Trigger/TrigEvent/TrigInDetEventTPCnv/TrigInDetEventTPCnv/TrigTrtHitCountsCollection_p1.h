/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrtHitCountsCollection_p1
 *
 * @brief persistent partner for TrigTrtHitCountsCollection
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_P1_H


#include "AthenaPoolUtilities/TPObjRef.h"  
#include <vector>
  
class TrigTrtHitCountsCollection_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigTrtHitCountsCollection_p1(){}
  friend class TrigTrtHitCountsCollectionCnv_p1;

 private:

  std::vector<TPObjRef> m_trtHitCounts;     

};// end of class definitions
 
 
#endif
