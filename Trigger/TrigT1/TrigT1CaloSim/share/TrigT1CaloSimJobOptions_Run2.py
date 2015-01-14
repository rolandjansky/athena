# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging  # loads logger

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2CPMTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2JetElementMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Tester

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMaker
job += Run2TriggerTowerMaker( 'Run2TriggerTowerMaker' )

log = logging.getLogger( "TrigT1CaloSimJobOptions_Run2" )
log.info("Scheduling CPMTowerMaker, JetElementMaker, CPMSim, JEMJetSim, JEMEnergySim, CPCMX, JetCMX, EnergyCMX, RoIROD, Tester")

job += LVL1__Run2CPMTowerMaker( 'CPMTowerMaker' )
job += LVL1__Run2JetElementMaker( 'JetElementMaker' )
job += LVL1__CPMSim( 'CPMSim' )
job += LVL1__JEMJetSim( 'JEMJetSim' )
job += LVL1__JEMEnergySim( 'JEMEnergySim' )
job += LVL1__CPCMX( 'CPCMX' )
job += LVL1__JetCMX( 'JetCMX' )
job += LVL1__EnergyCMX( 'EnergyCMX' )
job += LVL1__RoIROD( 'RoIROD' )
job += LVL1__Tester( 'Tester' )

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc, 'L1TriggerTowerTool'):
    ToolSvc += CfgMgr.LVL1__L1TriggerTowerTool('L1TriggerTowerTool')

job.Run2TriggerTowerMaker.ZeroSuppress = False

# autoconfigure pedestal correction based on the input file
_doPC = True
if not 'inputFileSummary' in dir():
    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
    except ImportError:
        pass

try:
    if not 'inputFileSummary' in dir():
        raise RuntimeError('Input file summary not available.')

    try:
        digiParam = inputFileSummary['metadata']['/Digitization/Parameters']
    except KeyError:
        raise RuntimeError('Collection /Digitization/Parameters not found in file metadata.')

    if 'intraTrainBunchSpacing' not in digiParam or not digiParam['intraTrainBunchSpacing']:
        raise RuntimeError('No key "intraTrainBunchSpacing" in /Digitization/Parameters.')
    _bunchSpacing = digiParam['intraTrainBunchSpacing']
except RuntimeError, re:
    log.warning('Autoconfiguration of pedestal correction failed: %s' % re)
    log.warning('Dynamic pedestal correction is disabled!')
    _doPC = False

if _doPC and _bunchSpacing not in (25,):
    log.warning('Only 25ns intra train bunch spacing supported. Dynamic pedestal correction is disabled!')
    _doPC = False

if _doPC:
    log.info('Configuring dynamic pedestal correction for %d ns bunch spacing.' % _bunchSpacing)
    ToolSvc.L1TriggerTowerTool.BaselineCorrection = True

    from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
    bct = BunchCrossingTool()
    if not hasattr(ToolSvc, bct.getName()):
        ToolSvc += bct
    else:
        bct = getattr(ToolSvc, bct.getName())

    if not hasattr(ToolSvc, 'L1DynamicPedestalProviderTxt'):
        ToolSvc += CfgMgr.LVL1__L1DynamicPedestalProviderTxt('L1DynamicPedestalProviderTxt',
                                                             BunchCrossingTool = bct,
                                                             InputFileEM_ShortGap='DynamicPedestalCorrection_SG_EM.txt',
                                                             InputFileHAD_ShortGap='DynamicPedestalCorrection_SG_HAD.txt',
                                                             InputFileEM_LongGap='DynamicPedestalCorrection_LG_EM.txt',
                                                             InputFileHAD_LongGap='DynamicPedestalCorrection_LG_HAD.txt')
    ToolSvc.L1TriggerTowerTool.L1DynamicPedestalProvider = ToolSvc.L1DynamicPedestalProviderTxt
else:
    job.Run2TriggerTowerMaker.BaselineCorrection = False
