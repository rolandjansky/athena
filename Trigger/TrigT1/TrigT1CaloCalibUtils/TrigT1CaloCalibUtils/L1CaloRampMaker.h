// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORAMPMAKER_H
#define L1CALORAMPMAKER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainer.h"
#include "CaloEvent/CaloCellContainer.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include <string>
#include <map>
#include <memory>

// forward declarations
class Identifier;
class L1CaloCondSvc;
class CaloLVL1_ID;
class L1CaloRampDataContainer;
class L1CaloRampMaker;
namespace LVL1{
  class IL1CaloOfflineTriggerTowerTools;
  class IL1TriggerTowerTool;
  class IL1CaloxAODOfflineTriggerTowerTools;
}

/**
 * The L1CaloRampMaker creates a set of L1CaloRampData objects.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 */
class L1CaloRampMaker : public AthAlgorithm
{
 public:
    L1CaloRampMaker(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~L1CaloRampMaker();
    
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

 private:
    // properties
    std::string m_triggerTowerContainerName; // name of transient TriggerTower container
    std::string m_outputFolderName;
    std::string m_gainStrategyFolder;
    std::map<int, int> m_specialChannelRange; // upper limit for coolIds (sick TBB)
    bool m_doLAr; // LAr calibration run
    bool m_doTile; // Tile calibration run
    bool m_isGain1; // correct energy for gain 1 runs
    bool m_isOvEmb; // Overlap Barrel
    bool m_isOvEmec; // Overlap EndCap
    bool m_isFcalLowEta; // FCAL low eta run
    bool m_isFcalHighEta; // FCAL high eta run
    bool m_checkProvenance; // check if OFC iteration failed
    unsigned int m_nEventsPerStep; // events per energy step
    unsigned int m_nSteps; // number of energy steps
    int m_fadcSaturationCut; // cut on FADC counts
    double m_tileSaturationCut; // cut on Tile TT Et in GeV
    ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool; // Handle to L1TriggerTowerTool.
    ToolHandle<LVL1::IL1CaloxAODOfflineTriggerTowerTools> m_xAODTTTools;
    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_jmTools;
    ServiceHandle<L1CaloCondSvc> m_condSvc;

    unsigned int m_nEvent;
    bool m_firstEvent;

    const CaloLVL1_ID *m_lvl1Helper;

    std::unique_ptr<L1CaloRampDataContainer> m_rampDataContainer;
    L1CaloPprConditionsContainerRun2 *m_pprLutContainer;
    //L1CaloPprDisabledChannelContainer *m_pprDisabledChannelContainer;

    // private functions
    void setupRampDataContainer(const xAOD::TriggerTowerContainer* triggerTowerContainer);
    double getTriggerTowerEnergy(const xAOD::TriggerTower* tt);
    double getCaloEnergy(const xAOD::TriggerTower* tt);
    void checkProvenance(const xAOD::TriggerTower* tt);

    // stores coolid vs. number of failed ofc iterations
    std::map<unsigned int, std::pair<unsigned int, double> > m_mapBadOFCIteration;

    std::map<L1CaloPprConditionsContainerRun2::eCoolFolders, std::string> m_pprLutContainerFolderMap;
    std::map<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string> m_pprDisabledChannelContainerFolderMap;

    struct DBInfo {
      unsigned int pedValue;
      bool disabledChannel;
    };
    std::map<uint32_t, DBInfo> m_tempDBInfo;

    SG::ReadHandleKey<CaloCellContainer> m_caloCellsKey
      { this, "CaloCellsKey", "AllCalo", "CaloCellContainer key" };
};

#endif //L1CALORAMPMAKER_H
