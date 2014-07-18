# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__TriggerTowerMaker
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
if globalflags.DetDescrVersion().startswith("Rome") or  globalflags.DetDescrVersion().startswith("DC2") or globalflags.DetDescrVersion().startswith("ATLAS-DC3-00") or globalflags.DetDescrVersion().startswith("ATLAS-DC3-01") or globalflags.DetDescrVersion().startswith("ATLAS-DC3-02"):
   from TrigT1CaloSim.TrigT1CaloSimConfig import TriggerTowerMaker_TTL1_Rel11
   triggerTowerMaker = TriggerTowerMaker_TTL1_Rel11( 'TriggerTowerMaker' )
   job += [triggerTowerMaker]
elif globalflags.DetDescrVersion().startswith("ATLAS-CSC-00") or globalflags.DetDescrVersion().startswith("ATLAS-CSC-01") or globalflags.DetDescrVersion().startswith("ATLAS-DC3"):
   from TrigT1CaloSim.TrigT1CaloSimConfig import TriggerTowerMaker_TTL1
   triggerTowerMaker = TriggerTowerMaker_TTL1( 'TriggerTowerMaker' )
   job += [triggerTowerMaker]
else:
   from TrigT1CaloSim.TrigT1CaloSimConfig import TriggerTowerMaker_TTL1_Rel13
   triggerTowerMaker = TriggerTowerMaker_TTL1_Rel13( 'TriggerTowerMaker' )
   job += [triggerTowerMaker]
   
job += LVL1__CPMTowerMaker( 'CPMTowerMaker' )
job += LVL1__JetElementMaker( 'JetElementMaker' )
job += LVL1__EmTauTrigger( 'EmTauTrigger' )
job += LVL1__JetTrigger( 'JetTrigger' )
job += LVL1__EnergyTrigger( 'EnergyTrigger' )
job += LVL1__ROD( 'ROD' )
job += LVL1__CPCMMMaker( 'CPCMMMaker' )
job += LVL1__JEPCMMMaker( 'JEPCMMMaker' )

job.TriggerTowerMaker.DoNotCalibrate=False
job.TriggerTowerMaker.MatchFIR=False
job.TriggerTowerMaker.EmBFilterCoeffs=[2,7,15,9,2]
job.TriggerTowerMaker.EmECFilterCoeffs=[2,7,15,9,2]
job.TriggerTowerMaker.FcalEFilterCoeffs=[0,3,15,5,0]
job.TriggerTowerMaker.TileFilterCoeffs=[1,8,15,10,4]
job.TriggerTowerMaker.HecFilterCoeffs=[0,9,15,11,6]
job.TriggerTowerMaker.FcalHFilterCoeffs=[0,4,15,8,0]
job.TriggerTowerMaker.EmBThresh=2048
job.TriggerTowerMaker.EmECThresh=2048
job.TriggerTowerMaker.FcalEThresh=2048
job.TriggerTowerMaker.TileThresh=2048
job.TriggerTowerMaker.HecThresh=2048
job.TriggerTowerMaker.FcalHThresh=2048

job.TriggerTowerMaker.BaselineCorrection = False
job.TriggerTowerMaker.PulseElements = False

from AthenaCommon.AppMgr import ToolSvc
from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1TriggerTowerTool
ToolSvc += LVL1__L1TriggerTowerTool('L1TriggerTowerTool')
ToolSvc.L1TriggerTowerTool.BaselineCorrection=False


