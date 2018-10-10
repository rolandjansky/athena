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
#include "TrigT1CaloSim/OverlayRun2TriggerTowerMaker.h"
#include "TrigT1CaloSim/TransientDatabaseOverride.h"
#include "TrigT1CaloSim/OverlayTTL1.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

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
DECLARE_ALGORITHM_FACTORY(OverlayRun2TriggerTowerMaker)
DECLARE_ALGORITHM_FACTORY(TransientDatabaseOverride)
DECLARE_ALGORITHM_FACTORY(OverlayTTL1)

DECLARE_FACTORY_ENTRIES(TrigT1CaloSim) {
    DECLARE_ALGORITHM(TriggerTowerMaker)
    DECLARE_ALGORITHM(Run2TriggerTowerMaker)
    DECLARE_ALGORITHM(CPMTowerMaker)
    DECLARE_ALGORITHM(JetElementMaker)
    DECLARE_ALGORITHM(Run2CPMTowerMaker)
    DECLARE_ALGORITHM(Run2JetElementMaker)
    DECLARE_ALGORITHM(MergeSlices)
    DECLARE_ALGORITHM(Tester)
    DECLARE_ALGORITHM(ReprocessTriggerTowers)
    DECLARE_ALGORITHM(CPMSim)
    DECLARE_ALGORITHM(JEMJetSim)
    DECLARE_ALGORITHM(JEMEnergySim)
    DECLARE_ALGORITHM(RoIROD)
    DECLARE_ALGORITHM(CPCMX)
    DECLARE_ALGORITHM(JetCMX)
    DECLARE_ALGORITHM(EnergyCMX)
    DECLARE_ALGORITHM(OverlayRun2TriggerTowerMaker)
    DECLARE_ALGORITHM(TransientDatabaseOverride)
    DECLARE_ALGORITHM(OverlayTTL1)
}

//Notes:
//
//1. The argument to the DECLARE_FACTORY_ENTRIES(XXX) is the name of the
//   component library (libXXX.so).
//2. Each Algorithm that is contained in the library should be declared
//   by the DECLARE_ALGORITHM() statement.
//3. Each Service that is contained in the library should be declared
//   by the DECLARE_SERVICE() statement.
//
// See Athena User Guide for more information
