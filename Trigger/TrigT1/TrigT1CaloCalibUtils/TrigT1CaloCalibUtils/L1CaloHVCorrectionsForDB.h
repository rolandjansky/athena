/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOHVCORRECTIONSFORDB_H
#define L1CALOHVCORRECTIONSFORDB_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <boost/scoped_ptr.hpp>

// forward declrations
class L1CaloRxLayersContainer;
class L1CaloHVCorrectionsContainer;

namespace LVL1{
  class IL1TriggerTowerTool;
  class IL1CaloOfflineTriggerTowerTools;
}

/**
 * This algorithms creates a sqlite database of current HV Corrections.
 *
 * @author Peter Faulkner <pjwf@hep.ph.bham.ac.uk>
 */

class L1CaloHVCorrectionsForDB : public AthAlgorithm
{
 public:
    L1CaloHVCorrectionsForDB(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~L1CaloHVCorrectionsForDB();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

 private:
    std::string m_triggerTowerCollectionName;
    std::string m_caloCellContainerName;

    ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_jmTools;
    SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey
    { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };
    SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    { this, "CablingKey", "LArOnOffIdMap", "SG Key of LArOnOffIdMapping object" };

    boost::scoped_ptr<L1CaloRxLayersContainer> m_rxLayersContainer;
    boost::scoped_ptr<L1CaloHVCorrectionsContainer> m_hvCorrectionsContainer;

    bool m_firstEvent;
};

#endif // L1CALOHVCORRECTIONSFORDB_H
