/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPBSCollection.h"

namespace LVL1 {

CPBSCollection::CPBSCollection(const DataVector<CPMTower>*  ttCollection,
                               const DataVector<CPMHits>*   hitCollection,
                               const DataVector<CMMCPHits>* cmmHitCollection)
                              : m_ttCollection(ttCollection),
			        m_hitCollection(hitCollection),
			        m_cmmHitCollection(cmmHitCollection)
{
}

} // end namespace
