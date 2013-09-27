/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCollection_p2
 *
 * @brief persistent partner for TrigSpacePointCountsCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCollection_p2.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_P2_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigSpacePointCountsCollection_p2 : public std::vector<TPObjRef>
{
 public:
    
  TrigSpacePointCountsCollection_p2(){}
  friend class TrigSpacePointCountsCollectionCnv_p2;

 private:

  std::vector<TPObjRef> m_trigSpacePointCounts;

};// end of class definitions
 
 
#endif
