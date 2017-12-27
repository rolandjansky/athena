/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGTRTHITCOUNTSCOLLECTION_H
#define TRIGINDETEVENT_TRIGTRTHITCOUNTSCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigTrtHitCounts.h"
#include "AthenaKernel/BaseInfo.h"

/** @class TrigTrtHitCountsCollection
 * @author Samir Ferrag <S.Ferrag@physics.gla.ac.uk>
 * 
 * @brief Container class needed for EDM.  Should normally only
 * contain 1 TrigTrtHitCounts object.
 *
 */

class TrigTrtHitCountsCollection : public DataVector<TrigTrtHitCounts> { }; 

// obtained using clid -m TrigTrtHitCountsCollection
CLASS_DEF( TrigTrtHitCountsCollection , 1246744021 , 1 )

SG_BASE(TrigTrtHitCountsCollection, DataVector<TrigTrtHitCounts>);

#endif 
