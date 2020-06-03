/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JEPBSCollectionV2.h"

namespace LVL1 {

JEPBSCollectionV2::JEPBSCollectionV2(
    const DataVector<JetElement>* jeCollection,
    const DataVector<JEMEtSums>* etCollection,
    const DataVector<CMXJetTob>* cmxTobCollection,
    const DataVector<CMXJetHits>* cmxHitCollection,
    const DataVector<CMXEtSums>* cmxEtCollection)
    : m_jeCollection(jeCollection),
      m_etCollection(etCollection),
      m_cmxTobCollection(cmxTobCollection),
      m_cmxHitCollection(cmxHitCollection),
      m_cmxEtCollection(cmxEtCollection) {}

} // end namespace
