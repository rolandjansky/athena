#include "TrigT1CaloSim/TriggerTowerMaker.h"
#include "TrigT1CaloSim/Run2TriggerTowerMaker.h"
#include "TrigT1CaloSim/CPMTowerMaker.h"
#include "TrigT1CaloSim/JetElementMaker.h"
#include "TrigT1CaloSim/Run2CPMTowerMaker.h"
#include "TrigT1CaloSim/Run2JetElementMaker.h"
#include "TrigT1CaloSim/MergeSlices.h"
#include "TrigT1CaloSim/Tester.h"
#include "TrigT1CaloSim/ReprocessTriggerTowers.h"
#include "TrigT1CaloSim/CPMSim.h"
#include "TrigT1CaloSim/JEMJetSim.h"
#include "TrigT1CaloSim/JEMEnergySim.h"
#include "TrigT1CaloSim/RoIROD.h"
#include "TrigT1CaloSim/CPCMX.h"
#include "TrigT1CaloSim/JetCMX.h"
#include "TrigT1CaloSim/EnergyCMX.h"
#include "TrigT1CaloSim/TransientDatabaseOverride.h"


using namespace LVL1;

DECLARE_COMPONENT( TriggerTowerMaker )
DECLARE_COMPONENT( Run2TriggerTowerMaker )
DECLARE_COMPONENT( CPMTowerMaker )
DECLARE_COMPONENT( JetElementMaker )
DECLARE_COMPONENT( Run2CPMTowerMaker )
DECLARE_COMPONENT( Run2JetElementMaker )
DECLARE_COMPONENT( MergeSlices )
DECLARE_COMPONENT( Tester )
DECLARE_COMPONENT( ReprocessTriggerTowers )
DECLARE_COMPONENT( CPMSim )
DECLARE_COMPONENT( JEMJetSim )
DECLARE_COMPONENT( JEMEnergySim )
DECLARE_COMPONENT( RoIROD )
DECLARE_COMPONENT( CPCMX )
DECLARE_COMPONENT( JetCMX )
DECLARE_COMPONENT( EnergyCMX )
DECLARE_COMPONENT( TransientDatabaseOverride )

