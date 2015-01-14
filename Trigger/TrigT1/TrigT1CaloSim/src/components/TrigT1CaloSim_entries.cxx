#include "TrigT1CaloSim/EmTauTrigger.h"
#include "TrigT1CaloSim/TriggerTowerMaker.h"
#include "TrigT1CaloSim/Run2TriggerTowerMaker.h"
#include "TrigT1CaloSim/CPMTowerMaker.h"
#include "TrigT1CaloSim/JetElementMaker.h"
#include "TrigT1CaloSim/Run2CPMTowerMaker.h"
#include "TrigT1CaloSim/Run2JetElementMaker.h"
#include "TrigT1CaloSim/JetTrigger.h"
#include "TrigT1CaloSim/EnergyTrigger.h"
#include "TrigT1CaloSim/CPCMMMaker.h"
#include "TrigT1CaloSim/JEPCMMMaker.h"
#include "TrigT1CaloSim/MergeSlices.h"
#include "TrigT1CaloSim/Tester.h"
#include "TrigT1CaloSim/ROD.h"
#include "TrigT1CaloSim/DumpTriggerObjects.h"
#include "TrigT1CaloSim/ReprocessTriggerTowers.h"
#include "TrigT1CaloSim/CPMSim.h"
#include "TrigT1CaloSim/JEMJetSim.h"
#include "TrigT1CaloSim/JEMEnergySim.h"
#include "TrigT1CaloSim/RoIROD.h"
#include "TrigT1CaloSim/CPCMX.h"
#include "TrigT1CaloSim/JetCMX.h"
#include "TrigT1CaloSim/EnergyCMX.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace LVL1;

DECLARE_ALGORITHM_FACTORY(TriggerTowerMaker)
DECLARE_ALGORITHM_FACTORY(Run2TriggerTowerMaker)
DECLARE_ALGORITHM_FACTORY(CPMTowerMaker)
DECLARE_ALGORITHM_FACTORY(JetElementMaker)
DECLARE_ALGORITHM_FACTORY(Run2CPMTowerMaker)
DECLARE_ALGORITHM_FACTORY(Run2JetElementMaker)
DECLARE_ALGORITHM_FACTORY(EmTauTrigger)
DECLARE_ALGORITHM_FACTORY(JetTrigger)
DECLARE_ALGORITHM_FACTORY(EnergyTrigger)
DECLARE_ALGORITHM_FACTORY(CPCMMMaker)
DECLARE_ALGORITHM_FACTORY(JEPCMMMaker)
DECLARE_ALGORITHM_FACTORY(MergeSlices)
DECLARE_ALGORITHM_FACTORY(Tester)
DECLARE_ALGORITHM_FACTORY(ROD)
DECLARE_ALGORITHM_FACTORY(ReprocessTriggerTowers)
DECLARE_ALGORITHM_FACTORY(DumpTriggerObjects)
DECLARE_ALGORITHM_FACTORY(CPMSim)
DECLARE_ALGORITHM_FACTORY(JEMJetSim)
DECLARE_ALGORITHM_FACTORY(JEMEnergySim)
DECLARE_ALGORITHM_FACTORY(RoIROD)
DECLARE_ALGORITHM_FACTORY(CPCMX)
DECLARE_ALGORITHM_FACTORY(JetCMX)
DECLARE_ALGORITHM_FACTORY(EnergyCMX)

DECLARE_FACTORY_ENTRIES(TrigT1CaloSim) {
    DECLARE_ALGORITHM(TriggerTowerMaker)
    DECLARE_ALGORITHM(Run2TriggerTowerMaker)
    DECLARE_ALGORITHM(CPMTowerMaker)
    DECLARE_ALGORITHM(JetElementMaker)
    DECLARE_ALGORITHM(Run2CPMTowerMaker)
    DECLARE_ALGORITHM(Run2JetElementMaker)
    DECLARE_ALGORITHM(EmTauTrigger )
    DECLARE_ALGORITHM(JetTrigger)
    DECLARE_ALGORITHM(EnergyTrigger)
    DECLARE_ALGORITHM(CPCMMMaker)
    DECLARE_ALGORITHM(JEPCMMMaker)
    DECLARE_ALGORITHM(MergeSlices)
    DECLARE_ALGORITHM(Tester)
    DECLARE_ALGORITHM(ROD)
    DECLARE_ALGORITHM(ReprocessTriggerTowers)
    DECLARE_ALGORITHM(DumpTriggerObjects)
    DECLARE_ALGORITHM(CPMSim)
    DECLARE_ALGORITHM(JEMJetSim)
    DECLARE_ALGORITHM(JEMEnergySim)
    DECLARE_ALGORITHM(RoIROD)
    DECLARE_ALGORITHM(CPCMX)
    DECLARE_ALGORITHM(JetCMX)
    DECLARE_ALGORITHM(EnergyCMX)
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
