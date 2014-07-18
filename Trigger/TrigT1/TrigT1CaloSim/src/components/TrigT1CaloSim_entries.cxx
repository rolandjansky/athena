#include "TrigT1CaloSim/EmTauTrigger.h"
#include "TrigT1CaloSim/TriggerTowerMaker.h"
#include "TrigT1CaloSim/CPMTowerMaker.h"
#include "TrigT1CaloSim/JetElementMaker.h"
#include "TrigT1CaloSim/JetTrigger.h"
#include "TrigT1CaloSim/EnergyTrigger.h"
#include "TrigT1CaloSim/CPCMMMaker.h"
#include "TrigT1CaloSim/JEPCMMMaker.h"
#include "TrigT1CaloSim/MergeSlices.h"
#include "TrigT1CaloSim/Tester.h"
#include "TrigT1CaloSim/ROD.h"
#include "TrigT1CaloSim/DumpTriggerObjects.h"
#include "TrigT1CaloSim/ReprocessTriggerTowers.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace LVL1;

DECLARE_ALGORITHM_FACTORY(TriggerTowerMaker)
DECLARE_ALGORITHM_FACTORY(CPMTowerMaker)
DECLARE_ALGORITHM_FACTORY(JetElementMaker)
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


DECLARE_FACTORY_ENTRIES(TrigT1CaloSim) {
    DECLARE_ALGORITHM(TriggerTowerMaker)
    DECLARE_ALGORITHM(CPMTowerMaker)
    DECLARE_ALGORITHM(JetElementMaker)
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
