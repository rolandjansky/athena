from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
 
from glob import glob

if not "InputFiles" in dir():
    #InputFiles = [ "/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/attila.AOD.pool.root" ]
    ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/r/rwhite/workspace/public/tutorial/data16_13TeV.00307195.physics_Main.merge.DAOD_EGZ.f731_m1616_f731_m1662._0001.1"]
if not "OutputFile" in dir():
    OutputFile = "TDTExample.root"
if not "AthenaCommon.AppMgr.EvtMax" in dir():
    theApp.EvtMax=20

cfgtool = CfgMgr.TrigConf__xAODConfigTool("xAODConfigTool")
ToolSvc += cfgtool

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool

ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

#########################################################################
#                                                                       #
#                      Now set up the example job                       #
#                                                                       #
#########################################################################
# Define sets of triggers to use

electronHLTList=["HLT_e26_lhtight_nod0_ivarloose","HLT_e26_lhtight_nod0_iloose","HLT_e17_lhloose_nod0"] 
tauHLTList=[]
muonHLTList=[]
jetHLTList=["HLT_j300","HLT_3j20"]
combinedList=[]
mergedList=electronHLTList + tauHLTList + muonHLTList + jetHLTList

# Add the additional tools required for running the example algorithms

# Helper tool, provides some simple template methods to dump Navigation of chains
ToolSvc += CfgMgr.Trig__TriggerAnalysisHelper("TriggerAnalysisHelper")

# Matching Tool
ToolSvc += CfgMgr.Trig__MatchingTool("MatchingTool",OutputLevel=DEBUG)

# Emulators
#
# Egamma emulator -- disabled for now
# from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationToolFactory
# egEmulator = TrigEgammaEmulationToolFactory(elHLTList, name = "TrigEgammaEmulationTool",  OutputLevel = INFO)

# Bunch crossing tool

#from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
#if isMC: bcTool = BunchCrossingTool( "MC" )
#else: bcTool = BunchCrossingTool( "LHC" )
#ToolSvc += bcTool
#
# Trigger Analysis Examples
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import Trig__TDTExample
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TagAndProbeExAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import JetMetExAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigEmulatorExAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigExpressStreamAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TriggerAnalysisTutorial
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigMatchExAlg

tdtexample = Trig__TDTExample( "TDTExample", TriggerList=["HLT_e26_lhtight_nod0_ivarloose","HLT_e26_lhtight_nod0_iloose","HLT_e17_lhloose_nod0"] )
match = TrigMatchExAlg()
tp_electron=TagAndProbeExAlg( "TagAndProbeExAlg", Flavor="Electron",
        HLTTriggerList=electronHLTList,
        L1TriggerList=["L1_EM15VH","L1_EM22VHI"])
jetmet = JetMetExAlg("JetMetExAlg",HLTTriggerList=['HLT_j300'])
emulator = TrigEmulatorExAlg(HLTTriggerList=electronHLTList)
express=TrigExpressStreamAlg()
tat = TriggerAnalysisTutorial()

# Histogram routing
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += ["Trigger DATAFILE='TriggerAnalysisTutorial.root' TYP='ROOT' OPT='RECREATE'"]
ServiceMgr.THistSvc.OutputLevel = ERROR

# Add the examples to the top algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += tdtexample
topSequence += match
topSequence += tp_electron
topSequence += jetmet
#topSequence += emulator
topSequence += express
#topSequence += tat
