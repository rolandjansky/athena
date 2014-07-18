# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags

from TrigT1CaloSim.TrigT1CaloSimConfig import TriggerTowerMaker_TTL1

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetElementMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EmTauTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ROD
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__MergeSlices
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMMMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEPCMMMaker

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Define number of LUT slices and pedestal value required
timeslices = 5
pedestal   = 32

# Get default StoreGate keys of the collections which will
# exist in multiple copies
triggerTowers = TriggerTowerMaker_TTL1.getDefaultProperty('TriggerTowerLocation')
cpmTowers     = LVL1__CPMTowerMaker.getDefaultProperty('CPMTowerLocation')
jetElements   = LVL1__JetElementMaker.getDefaultProperty('JetElementLocation')
cpmHits       = LVL1__EmTauTrigger.getDefaultProperty('CPMHitsLocation')
emTauRois     = LVL1__EmTauTrigger.getDefaultProperty('EmTauROILocation')
jemHits       = LVL1__JetTrigger.getDefaultProperty('JEMHitsLocation')
jetRois       = LVL1__JetTrigger.getDefaultProperty('JetROIOutputLocation')
jemEtSums     = LVL1__EnergyTrigger.getDefaultProperty('JEMEtSumsLocation')
energyRois    = LVL1__EnergyTrigger.getDefaultProperty('EnergyRoILocation')
energyCtps    = LVL1__EnergyTrigger.getDefaultProperty('EnergyCTPLocation')
emTauCtps     = LVL1__ROD.getDefaultProperty('EmTauCTPLocation')
emTauSlink    = LVL1__ROD.getDefaultProperty('EmTauSlinkLocation')
jepSlink      = LVL1__ROD.getDefaultProperty('JEPSlinkLocation')
energySlink   = LVL1__ROD.getDefaultProperty('EnergySlinkLocation')
jetCtps       = LVL1__ROD.getDefaultProperty('JetCTPLocation')
jetEtRois     = LVL1__ROD.getDefaultProperty('JetEtRoILocation')

# For those algorithms which can only deal with one slice at a time
# repeat for each slice using modified StoreGate keys.
# For RoI/CTP/SLink give triggered slice the default key so that
# they will be picked up correctly by later algorithms.
peak = timeslices / 2
for slice in range( timeslices ):
    sl = str(slice)
    slRoi = sl
    if slice == peak:
       slRoi = ''
    # The noise stuff is only actually used when slice = 0
    tt = TriggerTowerMaker_TTL1( 'TriggerTowerMaker' + sl )
    tt.TriggerTowerLocation    = triggerTowers + sl
    tt.TimeslicesLUT           = timeslices
    tt.SliceLUT                = slice
    tt.PedestalValue           = pedestal
    tt.TowerNoise              = True
    tt.NumberOfEmNoiseRegions  = 2
    tt.EmNoiseEtaLower         = [0.,1.5]
    tt.EmNoiseRMS              = [320.,690.]
    tt.NumberOfHadNoiseRegions = 2
    tt.HadNoiseEtaLower        = [0.,1.5]
    tt.HadNoiseRMS             = [450.,500.]
    job += tt
    job += LVL1__CPMTowerMaker( 'CPMTowerMaker' + sl,
             TriggerTowerLocation    = triggerTowers + sl,
             CPMTowerLocation        = cpmTowers + sl )
    job += LVL1__JetElementMaker( 'JetElementMaker' + sl,
             TriggerTowerLocation    = triggerTowers + sl,
             JetElementLocation      = jetElements + sl )
    job += LVL1__EmTauTrigger( 'EmTauTrigger' + sl,
             TriggerTowerLocation    = triggerTowers + sl,
             CPMHitsLocation         = cpmHits + sl,
             EmTauROILocation        = emTauRois + slRoi )
    job += LVL1__JetTrigger( 'JetTrigger' + sl,
             JetElementLocation      = jetElements + sl,
             JEMHitsLocation         = jemHits + sl,
             JetROIOutputLocation    = jetRois + slRoi )
    job += LVL1__EnergyTrigger( 'EnergyTrigger' + sl,
             JetElementLocation      = jetElements + sl,
             JEMEtSumsLocation       = jemEtSums + sl,
             EnergyRoILocation       = energyRois + slRoi,
             EnergyCTPLocation       = energyCtps + slRoi )
    job += LVL1__ROD( 'ROD' + sl,
             EmTauRoILocation        = emTauRois + slRoi,
             EmTauCTPLocation        = emTauCtps + slRoi,
             EmTauSlinkLocation      = emTauSlink + slRoi,
             JEPSlinkLocation        = jepSlink + slRoi,
             EnergySlinkLocation     = energySlink + slRoi,
             JetRoILocation          = jetRois + slRoi,
             JetCTPLocation          = jetCtps + slRoi,
             JetEtRoILocation        = jetEtRois + slRoi,
             EnergyRoILocation       = energyRois + slRoi )

# Merge what we have so far into single multi-slice collections
job += LVL1__MergeSlices( 'MergeSlices', TimeslicesLUT = timeslices )

# The remaining algorithms can process multiple slices.
job += LVL1__CPCMMMaker( 'CPCMMMaker' )
job += LVL1__JEPCMMMaker( 'JEPCMMMaker' )


job.TriggerTowerMaker.BaselineCorrection = False
job.TriggerTowerMaker.PulseElements = False

from AthenaCommon.AppMgr import ToolSvc
from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1TriggerTowerTool
ToolSvc += LVL1__L1TriggerTowerTool('L1TriggerTowerTool')
ToolSvc.L1TriggerTowerTool.BaselineCorrection=False


