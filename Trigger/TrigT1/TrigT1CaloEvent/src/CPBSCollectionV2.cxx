/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPBSCollectionV2.h"

namespace LVL1 {

CPBSCollectionV2::CPBSCollectionV2(const DataVector<CPMTower>*  ttCollection,
                                   const DataVector<CMXCPTob>*  tobCollection,
                                   const DataVector<CMXCPHits>* hitCollection)
                                   : m_ttCollection(ttCollection),
			             m_tobCollection(tobCollection),
			             m_hitCollection(hitCollection)
{
}

} // end namespace
