/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrackCountsCollection_tlp1
 *
 * @brief "top-level" persistent partner for TrigTrackCountsCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTrackCountsCollection_tlp1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCONTAINER_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCONTAINER_TLP1_H

#include "TrigInDetEventTPCnv/TrigTrackCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCounts_p2.h"

class TrigTrackCountsCollection_tlp1 
{
   
 public:
   
  TrigTrackCountsCollection_tlp1(){}
 
  std::vector<TrigTrackCountsCollection_p1>       m_trigTrackCountsCollectionVec;
  std::vector<TrigTrackCounts_p1>                m_trigTrackCountsVec;
  std::vector<TrigTrackCounts_p2>                m_trigTrackCountsVec_p2;
     
};//end of class definitions
 
#endif
