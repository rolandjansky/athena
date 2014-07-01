/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORAMPMAKER_H
#define L1CALORAMPMAKER_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloCalibConditions/L1CaloPprLutContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainer.h"
#include <string>
#include <map>


// forward declarations
class StoreGateSvc;
class L1CaloCondSvc;
class CaloLVL1_ID;
// class L1CaloCells2TriggerTowers;
class L1CaloRampDataContainer;
class L1CaloRampMaker;
// class L1CaloLArTowerEnergy;
// class TriggerTowerTools;

namespace LVL1{
  class IL1CaloCells2TriggerTowers;
  class IL1CaloLArTowerEnergy;
  class IL1CaloOfflineTriggerTowerTools;
}


/**
 * The L1CaloRampMaker creates a set of L1CaloRampData objects.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 */
class L1CaloRampMaker : public Algorithm
{
 public:
    L1CaloRampMaker(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~L1CaloRampMaker();
    
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

 private:
    // properties
    std::string m_triggerTowerCollectionName; // name of transient TriggerTower container
    std::string m_caloCellContainerName; // name of transient CaloCell container.
    std::string m_l1CaloCells2TriggerTowersToolName;
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
    ToolHandle<TileBeamInfoProvider> m_beamInfo; // Handle to TileBeamInfoProvider for CISPAR info.

    MsgStream m_log;
    unsigned int m_nEvent;
    bool m_firstEvent;

    StoreGateSvc *m_storeGate;
    StoreGateSvc *m_detStore;
    L1CaloCondSvc *m_condSvc;

    const CaloLVL1_ID *m_lvl1Helper;
//     L1CaloCells2TriggerTowers *m_cells2tt;
//     TriggerTowerTools* m_jmTools;

    ToolHandle<LVL1::IL1CaloCells2TriggerTowers> m_cells2tt;
    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_jmTools;

    L1CaloRampDataContainer *m_rampDataContainer;
    L1CaloPprLutContainer *m_pprLutContainer;
    L1CaloPprDisabledChannelContainer *m_pprDisabledChannelContainer;
//     L1CaloLArTowerEnergy *m_larEnergy;
    ToolHandle<LVL1::IL1CaloLArTowerEnergy> m_larEnergy;

    // private functions
    void setupRampDataContainer(const TriggerTowerCollection* triggerTowerCollection);
    double getTriggerTowerEnergy(const std::vector<int>& adc, const Identifier& id, unsigned int coolId, double eta);
    double getCaloEnergy(const Identifier& id, const TriggerTower* tt);
    

    // stores coolid vs. number of failed ofc iterations
    std::map<unsigned int, std::pair<unsigned int, double> > m_mapBadOFCIteration;
    void checkProvenance(const Identifier& ttId, unsigned int coolid);

    std::map<L1CaloPprLutContainer::eCoolFolders, std::string> m_pprLutContainerFolderMap;
    std::map<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string> m_pprDisabledChannelContainerFolderMap;
};

#endif //L1CALORAMPMAKER_H
