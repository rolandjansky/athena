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
    ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/r/rwhite/workspace/public/tutorial/data16_13TeV.00309640.physics_EnhancedBias.merge.AOD.r8611_r8612_p2777_tid09632341_00/AOD.09632341._000587.pool.root.1"]
if not "AthenaCommon.AppMgr.EvtMax" in dir():
    theApp.EvtMax=100

# Set some output limits
ServiceMgr.MessageSvc.infoLimit = 100000
ServiceMgr.MessageSvc.errorLimit = 10

# TrigDecisionTool configuration
from TrigAnalysisExamples import TDTAthAnalysisConfig

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
L1List=["L1_TAU12","L1_TAU12IM","L1_TAU60"]
mergedList=electronHLTList + tauHLTList + muonHLTList + jetHLTList

# Add the additional tools required for running the example algorithms

# Helper tool, provides some simple template methods to dump Navigation of chains
ToolSvc += CfgMgr.Trig__TriggerAnalysisHelper("TriggerAnalysisHelper")

#
# Trigger Analysis Examples
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import JetMetExAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigEmulatorExAlg

jetmet = JetMetExAlg("JetMetExAlg",HLTTriggerList=jetHLTList)
emulator = TrigEmulatorExAlg(HLTTriggerList=electronHLTList,L1TriggerList=L1List)

# Histogram routing
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += ["Trigger DATAFILE='TriggerAnalysisTutorial.root' TYP='ROOT' OPT='RECREATE'"]
ServiceMgr.THistSvc.OutputLevel = ERROR

# Add the examples to the top algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += jetmet
topSequence += emulator
