# set up messaging
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 10000000

# set up input
#inputfile = '/afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/x86_64-slc6-gcc47-opt/AtlasProduction/Tools/FullChainTests/ChainJobwthPileUp/MC12.147818.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.RDO.pool.root'
inputfile = '/afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/x86_64-slc6-gcc47-opt/AtlasProduction/Tools/FullChainTests/ChainJobwthPileUp/MC12.147802.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.RDO.pool.root'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ inputfile ]
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.PoolSvc.AttemptCatalogPatch=True 
svcMgr.EventSelector.InputCollections = [ inputfile ]

# disable writing of everything
from RecExConfig.RecFlags import rec
rec.doWriteAOD = False
rec.doWriteBS = False
rec.doWriteCalibHits = False
rec.doWriteESD = False
rec.doWriteRDO = False
rec.doWriteTAG = False
rec.doWriteTAGCOM = False
rec.doWritexAOD = False


# set up the autoconfiguration of conditions, geometry, etc
from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys(['everything'])


# Detector flags for setting up geometry
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.Print()
include("RecExCond/AllDet_detDescr.py")



# set up the simulation chain
# use TriggerGetter but disable HLT
from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
TF.doLVL1 = True
TF.doLVL2 = False
TF.doEF = False
TF.doHLT = False
TF.readLVL1configFromXML=True
TF.triggerMenuSetup = "MC_pp_v5"
 
from TriggerJobOpts.TriggerGetter import TriggerGetter
triggerGetter = TriggerGetter()


# alternatively one could do the steps, but this has not been tested thoroughly
if False:

    # set up the trigger configuration
    from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc, TrigConfigSvc, findFileInXMLPATH
    svcMgr += TrigConfigSvc()
    svcMgr.TrigConfigSvc.PriorityList = ["xml","ds"]

    svcMgr += LVL1ConfigSvc()
    svcMgr.LVL1ConfigSvc.ConfigSource = "XML"
    svcMgr.LVL1ConfigSvc.XMLMenuFile = findFileInXMLPATH("TriggerMenuXML/LVL1config_MC_pp_v5.xml")
    svcMgr.LVL1ConfigSvc.OutputLevel = VERBOSE
    from TriggerJobOpts.Lvl1TriggerOnlineGetter import Lvl1SimulationGetter
    lvl1 = Lvl1SimulationGetter()

    from TriggerJobOpts.Lvl1TriggerOnlineGetter import Lvl1SimulationGetter
    lvl1 = Lvl1SimulationGetter()


# setup CTP Simulation details
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.CTPSimulation.UseDeadTime = False
topSequence.CTPSimulation.OutputLevel = VERBOSE
topSequence.CTPSimulation.CtpVersion = 4


theApp.EvtMax = -1

