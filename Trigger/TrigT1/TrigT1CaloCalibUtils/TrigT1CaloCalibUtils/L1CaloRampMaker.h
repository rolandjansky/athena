// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORAMPMAKER_H
#define L1CALORAMPMAKER_H
#include "StoreGate/CondHandleKeyArray.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h"


#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include <string>
#include <map>
#include <memory>

// forward declarations
class Identifier;
class CaloLVL1_ID;
class L1CaloRampDataContainer;
class L1CaloRampMaker;
namespace LVL1{
  class IL1CaloOfflineTriggerTowerTools;
  class IL1TriggerTowerToolRun3;
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
    ToolHandle<LVL1::IL1TriggerTowerToolRun3> m_ttTool; // Handle to L1TriggerTowerTool.


    ToolHandle<LVL1::IL1CaloxAODOfflineTriggerTowerTools> m_xAODTTTools;
    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_jmTools;
 
  
    unsigned int m_nEvent;
    bool m_firstEvent;
  
    const CaloLVL1_ID *m_lvl1Helper;

    std::unique_ptr<L1CaloRampDataContainer> m_rampDataContainer;
  
    void setupRampDataContainer(const xAOD::TriggerTowerContainer* triggerTowerContainer);
    double getTriggerTowerEnergy(const xAOD::TriggerTower* tt, SG::ReadCondHandle<L1CaloPprChanCalibContainer> pprCond);
    double getCaloEnergy(const xAOD::TriggerTower* tt);
    void checkProvenance(const xAOD::TriggerTower* tt);
  

  // stores coolid vs. number of failed ofc iterations       
  std::map<unsigned int, std::pair<unsigned int, double> > m_mapBadOFCIteration;   
  
  SG::ReadCondHandleKey<CondAttrListCollection> m_strategy{ this, "TRIGGER", "/TRIGGER/Receivers/Conditions/Strategy", "Strategy" };
  
  SG::ReadCondHandleKey<L1CaloPprChanCalibContainer>  m_pprChanCalibContainer{ this, "InputKeyCond", "L1CaloPprChanCalibContainer"}; 
  SG::ReadCondHandleKey<L1CaloPprDisabledChannelContainerRun2> m_pprDisabledChannelContainer{ this, "InputKeyDisabledChannel", "L1CaloPprDisabledChannelContainerRun2"};
  

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};           
  SG::ReadHandleKey<CaloCellContainer> m_caloCellsKey{ this, "CaloCellsKey", "AllCalo", "CaloCellContainer key" };
  
  

  
  







};

#endif //L1CALORAMPMAKER_H
