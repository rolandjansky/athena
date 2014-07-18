# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ReprocessTriggerTowers
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetElementMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EmTauTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMMMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEPCMMMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ROD
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Tester
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__DumpTriggerObjects

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add the required algorithms to the sequence
job += LVL1__ReprocessTriggerTowers( 'ReprocessTriggerTowers' ) 
job += LVL1__CPMTowerMaker( 'CPMTowerMaker' )
job += LVL1__JetElementMaker( 'JetElementMaker' )
job += LVL1__EmTauTrigger( 'EmTauTrigger' )
job += LVL1__JetTrigger( 'JetTrigger' )
job += LVL1__EnergyTrigger( 'EnergyTrigger' )
job += LVL1__ROD( 'ROD' )
job += LVL1__CPCMMMaker( 'CPCMMMaker' )
job += LVL1__JEPCMMMaker( 'JEPCMMMaker' )

job.ReprocessTriggerTowers.EMBScale=[1.0013,1.0001,0.9993,0.9979,1.0009,1.0006,1.0069,1.0192,1.0574,1.0615,1.0717,1.0796,1.0724,1.0893,1.0]
job.ReprocessTriggerTowers.EMECScale=[1.0033,1.0905,0.9860,1.0123,1.0048,0.9956,1.0017,1.0040,1.0098,1.0130,1.00001,1.00001,1.00001,1.00001,1.00001]
job.ReprocessTriggerTowers.FCAL1Scale=[1.00001,1.00001,1.00001,1.00001]
job.ReprocessTriggerTowers.LUTStrategy = 1
job.CPMTowerMaker.TriggerTowerLocation="ReprocessedTriggerTowers"
job.EmTauTrigger.TriggerTowerLocation="ReprocessedTriggerTowers"
job.JetElementMaker.TriggerTowerLocation="ReprocessedTriggerTowers"


job.TriggerTowerMaker.BaselineCorrection = False
job.TriggerTowerMaker.PulseElements = False

from AthenaCommon.AppMgr import ToolSvc
from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1TriggerTowerTool
ToolSvc += LVL1__L1TriggerTowerTool('L1TriggerTowerTool')
ToolSvc.L1TriggerTowerTool.BaselineCorrection=False


