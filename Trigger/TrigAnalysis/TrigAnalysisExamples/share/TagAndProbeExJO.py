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
L1List=["L1_EM15VH","L1_EM22VHI"]

# Trigger Analysis Examples
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TagAndProbeExAlg

tp_electron=TagAndProbeExAlg( "TagAndProbeExAlg", Flavor="Electron",
        HLTTriggerList=electronHLTList,
        L1TriggerList=L1List)

# Histogram routing
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += ["Trigger DATAFILE='TriggerAnalysisTutorial.root' TYP='ROOT' OPT='RECREATE'"]
ServiceMgr.THistSvc.OutputLevel = ERROR

# Add the examples to the top algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += tp_electron
