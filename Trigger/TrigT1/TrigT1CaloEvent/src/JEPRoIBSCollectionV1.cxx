/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JEPRoIBSCollectionV1.h"

namespace LVL1 {

JEPRoIBSCollectionV1::JEPRoIBSCollectionV1(
                    const DataVector<JEMRoI>*     jemRoiCollection,
		    const CMMRoI*                 cmmRoi,
                    const DataVector<CMMJetHits>* cmmHitCollection,
		    const DataVector<CMMEtSums>*  cmmEtCollection)
                    : m_jemRoiCollection(jemRoiCollection),
		      m_cmmRoi(cmmRoi),
		      m_cmmHitCollection(cmmHitCollection),
		      m_cmmEtCollection(cmmEtCollection)
{
}

} // end namespace
