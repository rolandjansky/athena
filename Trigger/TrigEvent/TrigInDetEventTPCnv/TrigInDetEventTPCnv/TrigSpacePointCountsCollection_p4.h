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
 * $Id: TrigSpacePointCountsCollection_p4.h 365974 2011-05-16 12:01:04Z salvator $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_P4_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_P4_H

#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p4.h"
// #include <vector>
  
class TrigSpacePointCountsCollection_p4 : public std::vector<TrigSpacePointCounts_p4> { };
// end of class definitions
 
 
#endif
