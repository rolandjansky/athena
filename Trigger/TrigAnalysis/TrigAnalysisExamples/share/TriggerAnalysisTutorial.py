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
    ServiceMgr.EventSelector.InputCollections = ["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/rel_2/21.0.X/x86_64-slc6-gcc49-opt/offline/TrigEgammaValidation/RDOtoAOD_MC_transform_Zee_25ns_pileup/AOD.Zee.25ns.pileup.pool.root"]
if not "OutputFile" in dir():
    OutputFile = "TDTExample.root"
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

HLTList=["HLT_e26_lhtight_nod0_ivarloose","HLT_e26_lhtight_nod0_iloose","HLT_e17_lhloose_nod0"] 

# Trigger Analysis Examples
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TriggerAnalysisTutorial

tutorial=TriggerAnalysisTutorial( "TriggerAnalysisTutorial", TriggerList=HLTList)

# Histogram routing
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += ["Trigger DATAFILE='TriggerAnalysisTutorial.root' TYP='ROOT' OPT='RECREATE'"]
ServiceMgr.THistSvc.OutputLevel = ERROR

# Add the examples to the top algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += tutorial
