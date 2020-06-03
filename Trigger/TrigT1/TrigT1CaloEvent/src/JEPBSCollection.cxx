/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JEPBSCollection.h"

namespace LVL1 {

JEPBSCollection::JEPBSCollection(
                 const DataVector<JetElement>* jeCollection,
                 const DataVector<JEMHits>*    hitCollection,
		 const DataVector<JEMEtSums>*  etCollection,
                 const DataVector<CMMJetHits>* cmmHitCollection,
		 const DataVector<CMMEtSums>*  cmmEtCollection)
                 : m_jeCollection(jeCollection),
		   m_hitCollection(hitCollection),
		   m_etCollection(etCollection),
		   m_cmmHitCollection(cmmHitCollection),
		   m_cmmEtCollection(cmmEtCollection)
{
}

} // end namespace
