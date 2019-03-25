# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( "TrigT1CaloSimJobOptions_Run2" )

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2TriggerTowerMaker
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

include('TrigT1CaloCalibConditions/L1CaloCalibConditionsMC_jobOptions.py')
from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMaker25ns, Run2TriggerTowerMaker50ns

from SGComps.AddressRemappingSvc import addInputRename
addInputRename ( 'xAOD::TriggerTowerContainer', 'xAODTriggerTowers_rerun', 'xAODTriggerTowers')

# try to determine wheter running with 25ns/50ns bunchspacing
_bunchSpacing = None
_doPC = True # do pedestal correction?
_alg = {25 : Run2TriggerTowerMaker25ns, 50 : Run2TriggerTowerMaker50ns}
if not 'inputFileSummary' in dir():
    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
    except ImportError:
        pass
      
from AthenaCommon.GlobalFlags import globalflags
if globalflags.isOverlay() is True:
    _doPC = False
    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, "LumiBlockMuTool"):
        ToolSvc += CfgMgr.LumiBlockMuTool("LumiBlockMuTool")
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__OverlayRun2TriggerTowerMaker
    job += LVL1__OverlayRun2TriggerTowerMaker('Run2TriggerTowerMaker', 
                                      CellType = 3, 
                                      ZeroSuppress = True, 
                                      DoOverlay = True )
else:
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
        log.warning('Could not determine bunch-spacing from input file: %s' % re)
        log.warning('Configuring for 25ns w/o pedestal correction - a wrong configuration might yield non sensible results!')
        _bunchSpacing = 25
        _doPC = False # not enough information to configure pedestal correction

    if _bunchSpacing in _alg:
        log.info("Scheduling %s" %  _alg[_bunchSpacing].__name__)
        job += _alg[_bunchSpacing]( 'Run2TriggerTowerMaker' )
    else:
        log.warning('No tuned configuration for a bunch-spacing of %s available. Using 25ns settings w/o pedestal correction.' % _bunchSpacing)
        job += _alg[25]( 'Run2TriggerTowerMaker' )
        _doPC = False

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
#job += LVL1__Tester( 'Tester' )

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc, 'L1TriggerTowerTool'):
    ToolSvc += CfgMgr.LVL1__L1TriggerTowerTool('L1TriggerTowerTool')

job.Run2TriggerTowerMaker.ZeroSuppress = True

# autoconfigure pedestal correction based on the input file
if _doPC and _bunchSpacing not in (25,50):
    log.warning('Only 25ns intra train bunch spacing currently supported. Dynamic pedestal correction is disabled!')
    _doPC = False

ToolSvc.L1TriggerTowerTool.BaselineCorrection = _doPC

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
bct = BunchCrossingTool()
if not hasattr(ToolSvc, bct.getName()):
    ToolSvc += bct
else:
    bct = getattr(ToolSvc, bct.getName())
    
if _doPC and not hasattr(ToolSvc, 'L1DynamicPedestalProviderTxt'):
    ToolSvc += CfgMgr.LVL1__L1DynamicPedestalProviderTxt('L1DynamicPedestalProviderTxt',
                                                         BunchCrossingTool = bct,
                                                         InputFileEM_ShortGap='DynamicPedestalCorrection_SG_EM_%dns.txt' % _bunchSpacing,
                                                         InputFileHAD_ShortGap='DynamicPedestalCorrection_SG_HAD_%dns.txt' % _bunchSpacing,
                                                         InputFileEM_LongGap='DynamicPedestalCorrection_LG_EM_%dns.txt' % _bunchSpacing,
                                                         InputFileHAD_LongGap='DynamicPedestalCorrection_LG_HAD_%dns.txt' % _bunchSpacing)
    ToolSvc.L1TriggerTowerTool.L1DynamicPedestalProvider = ToolSvc.L1DynamicPedestalProviderTxt
