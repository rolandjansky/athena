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
from TrigT1CaloSim.TrigT1CaloSimConfig import TriggerTowerMaker_TTL1_Gain1
triggerTowerMaker = TriggerTowerMaker_TTL1_Gain1( 'TriggerTowerMaker' )
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
job.TriggerTowerMaker.LUTStrategy=0
job.TriggerTowerMaker.EmBThresh=4
job.TriggerTowerMaker.EmECThresh=4
job.TriggerTowerMaker.FcalEThresh=4
job.TriggerTowerMaker.TileThresh=4
job.TriggerTowerMaker.HecThresh=4
job.TriggerTowerMaker.FcalHThresh=4
job.TriggerTowerMaker.EmBFilterCoeffs=[0,7,15,10,0]
job.TriggerTowerMaker.EmECFilterCoeffs=[0,7,15,10,0]
job.TriggerTowerMaker.FcalEFilterCoeffs=[0,7,15,10,0]
job.TriggerTowerMaker.TileFilterCoeffs=[0,7,15,10,0]
job.TriggerTowerMaker.HecFilterCoeffs=[0,7,15,10,0]
job.TriggerTowerMaker.FcalHFilterCoeffs=[0,7,15,10,0]
job.TriggerTowerMaker.AutoCalibrateLUT=False
job.TriggerTowerMaker.EmReg1Slope = [1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.]
job.TriggerTowerMaker.EmReg2Slope = [ 1152.,1152.,1152. ]
job.TriggerTowerMaker.EmReg3Slope = [1152.]
job.TriggerTowerMaker.EmFwdSlope = [1152.,1152.,1152.,1152.]
job.TriggerTowerMaker.HadReg1Slope = [1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.,1152.]
job.TriggerTowerMaker.HadReg2Slope = [1152.,1152.,1152.]
job.TriggerTowerMaker.HadReg3Slope = [1152.]
job.TriggerTowerMaker.HadFwdSlope = [1152.,1152.,1152.,1152.]


job.TriggerTowerMaker.BaselineCorrection = False
job.TriggerTowerMaker.PulseElements = False

from AthenaCommon.AppMgr import ToolSvc
from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1TriggerTowerTool
ToolSvc += LVL1__L1TriggerTowerTool('L1TriggerTowerTool')
ToolSvc.L1TriggerTowerTool.BaselineCorrection=False


