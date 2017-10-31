
#include "TrigT1CaloCalibTools/L1CaloTTIdTools.h"
#include "TrigT1CaloCalibTools/L1CaloFcal23Cells2RxMappingTool.h"
#include "TrigT1CaloCalibTools/L1CaloCells2TriggerTowers.h"
#include "TrigT1CaloCalibTools/L1CaloLArTowerEnergy.h"
#include "TrigT1CaloCalibTools/L1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloCalibTools/L1CaloMonitoringCaloTool.h"
#include "TrigT1CaloCalibTools/TriggerTowerThinningAlg.h"
#include "TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg.h"
#include "TrigT1CaloCalibTools/L1CaloxAODOfflineTriggerTowerTools.h"

DECLARE_COMPONENT( LVL1::L1CaloTTIdTools )
DECLARE_COMPONENT( LVL1::L1CaloFcal23Cells2RxMappingTool )
DECLARE_COMPONENT( LVL1::L1CaloCells2TriggerTowers )
DECLARE_COMPONENT( LVL1::L1CaloLArTowerEnergy )
DECLARE_COMPONENT( LVL1::L1CaloOfflineTriggerTowerTools )
DECLARE_COMPONENT( LVL1::L1CaloMonitoringCaloTool )
DECLARE_COMPONENT( LVL1::L1CaloxAODOfflineTriggerTowerTools )
DECLARE_COMPONENT( DerivationFramework::TriggerTowerThinningAlg )
DECLARE_COMPONENT( LVL1::L1CaloTriggerTowerDecoratorAlg )

