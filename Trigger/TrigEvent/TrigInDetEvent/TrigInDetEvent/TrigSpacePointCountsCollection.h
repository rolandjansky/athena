/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGSPACEPOINTCOUNTSCOLLECTION_H
#define TRIGINDETEVENT_TRIGSPACEPOINTCOUNTSCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "AthenaKernel/BaseInfo.h"

/** @class TrigSctSpCountsCollection
 * @author Regina Kwee <Regina.Kwee@cern.ch>
 * 
 * @brief Container class needed for EDM.  Should normally only
 * contain 1 TrigSpacePointCounts object.
 *
 */
class TrigSpacePointCountsCollection : public DataVector<TrigSpacePointCounts> { }; 

// obtained using clid -m TrigSctSpCountsCollection
CLASS_DEF( TrigSpacePointCountsCollection , 1123027134 , 1 )

SG_BASE(TrigSpacePointCountsCollection, DataVector<TrigSpacePointCounts>);

#endif 
