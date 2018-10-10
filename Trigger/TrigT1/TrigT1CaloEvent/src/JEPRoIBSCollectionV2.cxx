/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1CaloEvent/JEPRoIBSCollectionV2.h"

namespace LVL1 {

JEPRoIBSCollectionV2::JEPRoIBSCollectionV2(
                    const DataVector<JEMTobRoI>*  jemRoiCollection,
		    const CMXRoI*                 cmxRoi,
		    const DataVector<CMXEtSums>*  cmxEtCollection)
                    : m_jemRoiCollection(jemRoiCollection),
		      m_cmxRoi(cmxRoi),
		      m_cmxEtCollection(cmxEtCollection)
{
}

} // end namespace
