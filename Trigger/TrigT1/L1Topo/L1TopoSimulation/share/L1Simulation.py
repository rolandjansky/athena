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

inputfile = '/afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/x86_64-slc6-gcc47-opt/AtlasProduction/Tools/FullChainTests/ChainJobwthPileUp/MC12.147818.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.RDO.pool.root'


rec.readRDO   = True
rec.doESD     = True
rec.doTrigger = True

globalflags.InputFormat = 'pool'
athenaCommonFlags.PoolRDOInput = [ inputfile ]
athenaCommonFlags.FilesInput = [ inputfile ]

athenaCommonFlags.EvtMax=-1

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
TriggerFlags.doHLT    = False

TriggerFlags.triggerMenuSetup = "MC_pp_v5"

TriggerFlags.inputL1TopoConfigFile = globals()['L1TopoMenu'] if 'L1TopoMenu' in globals() else 'TriggerMenuXML/L1Topoconfig_Physics_pp_v5_19.1.1.xml'
TriggerFlags.readL1TopoConfigFromXML = True
TriggerFlags.readLVL1configFromXML   = True
TriggerFlags.readHLTconfigFromXML    = True


include ("L1TopoSimulation/L1Simulation_topOptions.py")


# set some debug
from AthenaCommon.AlgSequence import AlgSequence
top = AlgSequence()
top.OutputLevel = DEBUG

top.TriggerTowerMaker.OutputLevel=DEBUG
top.CPMTowerMaker.OutputLevel=VERBOSE
top.EmTauTrigger.OutputLevel=VERBOSE
top.JetTrigger.OutputLevel=VERBOSE
top.EnergyTrigger.OutputLevel=VERBOSE
top.L1TopoSimulation.OutputLevel=VERBOSE
top.L1TopoSimulation.EnableInputDump = True
top.L1TopoSimulation.InputDumpFile = "inputdump.txt"

top.CTPSimulation.OutputLevel=VERBOSE
#top.RoIBuilder.OutputLevel=VERBOSE

print top.CTPSimulation
print top.L1TopoSimulation
