# the logger
from AthenaCommon.Logging import logging
log = logging.getLogger('L1Simulation.py')

# the flags
include("RecExCommon/RecoUsefulFlags.py")

###from RecExConfig.RecFlags import rec
###from AthenaCommon.GlobalFlags import globalflags
###from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags
###
###runArgs={}

# input 
#inputfile = '/afs/cern.ch/atlas/offline/ReleaseData/v16/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root'
#inputfile = '/afs/cern.ch/work/s/stelzer/LegacySimWorkareaForWernerJoerg/digitizedRDO/digiRDO.pool.root'

#inputfile = '/afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/x86_64-slc6-gcc47-opt/AtlasProduction/Tools/FullChainTests/ChainJobwthPileUp/MC12.108351.pythia_minbias.8TeV.RDO.pool.root'

#inputfile = '/afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/x86_64-slc6-gcc47-opt/AtlasProduction/Tools/FullChainTests/ChainJobwthPileUp/MC12.147818.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.RDO.pool.root'

# 1000 events
inputfile = '/afs/cern.ch/work/s/stelzer/mc11_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1272_s1274_d580_tid541908_00/RDO.541908._000001.pool.root.1'

rec.readRDO   = True
rec.doESD     = True
rec.doTrigger = True

globalflags.InputFormat = 'pool'
athenaCommonFlags.PoolRDOInput = [ inputfile ]
athenaCommonFlags.FilesInput = [ inputfile ]

athenaCommonFlags.EvtMax=100

rec.OutputLevel = WARNING
rec.Production  = False
rec.AutoConfiguration=['everything']

# production option
rec.Production = False
rec.doDumpPoolInputContent = True

include ( "RecExCond/RecExCommon_flags.py" )



TriggerFlags.doL1Topo = True
TriggerFlags.doLVL1   = True
TriggerFlags.doLVL2   = False
TriggerFlags.doEF     = False
TriggerFlags.doHLT    = True

TriggerFlags.triggerMenuSetup = "MC_pp_v5"


TriggerFlags.readL1TopoConfigFromXML = True
TriggerFlags.readLVL1configFromXML   = True
TriggerFlags.readHLTconfigFromXML    = True


TriggerFlags.inputL1TopoConfigFile = globals()['L1TopoMenu'] if 'L1TopoMenu' in globals() else 'TriggerMenuXML/L1Topoconfig_Physics_pp_v5_19.2.0.xml'
TriggerFlags.inputLVL1configFile = globals()['L1Menu'] if 'L1Menu' in globals() else 'TriggerMenuXML/LVL1config_Physics_pp_v5_19.2.0.xml'
TriggerFlags.inputHLTconfigFile = globals()['HLTMenu'] if 'HLTMenu' in globals() else 'TriggerMenuXML/HLTconfig_Physics_pp_v5_19.2.0.xml'

TriggerFlags.enableMonitoring = [ 'Validation', 'Online' ]


from TrigSteeringTest.TrigSteeringTestConf import DummyFexUsingTopo
dummyfex = DummyFexUsingTopo("test")

include ("L1TopoSimulation/L1Simulation_topOptions.py")


# set some debug
from AthenaCommon.AlgSequence import AlgSequence
top = AlgSequence()
top.OutputLevel = DEBUG

#top.TriggerTowerMaker.OutputLevel=DEBUG
#top.CPMTowerMaker.OutputLevel=VERBOSE
#top.EmTauTrigger.OutputLevel=VERBOSE
#top.JetTrigger.OutputLevel=VERBOSE
#top.EnergyTrigger.OutputLevel=VERBOSE
top.L1TopoSimulation.OutputLevel=VERBOSE
top.L1TopoSimulation.TopoOutputLevel=DEBUG
top.L1TopoSimulation.TopoSteeringOutputLevel=DEBUG
top.L1TopoSimulation.EnableInputDump = True
top.L1TopoSimulation.InputDumpFile = "inputdump.txt"
top.L1TopoSimulation.UseBitwise = False

# new calo sim
top.CPCMX.OutputLevel = VERBOSE
top.JetCMX.OutputLevel = VERBOSE
top.EnergyCMX.OutputLevel = VERBOSE

top.CTPSimulation.OutputLevel=VERBOSE
#top.RoIBuilder.OutputLevel=VERBOSE

top.TrigSteer_HLT.OutputLevel = DEBUG
top.TrigSteer_HLT.TopoOutputLevel = DEBUG



print top.CTPSimulation
print top.L1TopoSimulation
print top.L1TopoSimulation.EnergyInputProvider
print top.L1TopoSimulation.JetInputProvider
print top.L1TopoSimulation.EMTAUInputProvider
print top.L1TopoSimulation.MuonInputProvider
print top.CPCMX
print top.JetCMX
print top.EnergyCMX
print top.TrigSteer_HLT

print svcMgr.MessageSvc

svcMgr.MessageSvc.enableSuppression =False

svcMgr.MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents



#import sys
#sys.exit(0)
