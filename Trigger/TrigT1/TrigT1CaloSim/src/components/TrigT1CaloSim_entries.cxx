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

DECLARE_ALGORITHM_FACTORY(TriggerTowerMaker)
DECLARE_ALGORITHM_FACTORY(Run2TriggerTowerMaker)
DECLARE_ALGORITHM_FACTORY(CPMTowerMaker)
DECLARE_ALGORITHM_FACTORY(JetElementMaker)
DECLARE_ALGORITHM_FACTORY(Run2CPMTowerMaker)
DECLARE_ALGORITHM_FACTORY(Run2JetElementMaker)
DECLARE_ALGORITHM_FACTORY(MergeSlices)
DECLARE_ALGORITHM_FACTORY(Tester)
DECLARE_ALGORITHM_FACTORY(ReprocessTriggerTowers)
DECLARE_ALGORITHM_FACTORY(CPMSim)
DECLARE_ALGORITHM_FACTORY(JEMJetSim)
DECLARE_ALGORITHM_FACTORY(JEMEnergySim)
DECLARE_ALGORITHM_FACTORY(RoIROD)
DECLARE_ALGORITHM_FACTORY(CPCMX)
DECLARE_ALGORITHM_FACTORY(JetCMX)
DECLARE_ALGORITHM_FACTORY(EnergyCMX)
DECLARE_ALGORITHM_FACTORY(TransientDatabaseOverride)

