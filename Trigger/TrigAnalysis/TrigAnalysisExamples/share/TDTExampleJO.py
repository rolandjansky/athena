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
    #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/r/rwhite/workspace/public/tutorial/data16_13TeV.00307195.physics_Main.merge.DAOD_EGZ.f731_m1616_f731_m1662._0001.1"]
    ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/r/rwhite/workspace/public/tutorial/data16_13TeV.00309640.physics_EnhancedBias.merge.AOD.r8611_r8612_p2777_tid09632341_00/AOD.09632341._000587.pool.root.1"]
    #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/r/rwhite/workspace/public/tutorial/AOD.pool.root"]
if not "AthenaCommon.AppMgr.EvtMax" in dir():
    theApp.EvtMax=100

# Set some output limits
ServiceMgr.MessageSvc.infoLimit = 100000
ServiceMgr.MessageSvc.errorLimit = 10

# TrigDecisionTool configuration
from TrigAnalysisExamples import TDTAthAnalysisConfig

# TrigDecisionTool Example algorithm
from TrigAnalysisExamples.TrigAnalysisExamplesConf import Trig__TDTExample

tdtexample = Trig__TDTExample( "TDTExample", 
        TriggerList=["HLT_e26_lhtight_nod0_ivarloose",
            "HLT_e26_lhtight_nod0_iloose",
            "HLT_e17_lhloose_nod0",
            "HLT_j150",
            "HLT_xe50"] )

# Add the examples to the top algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += tdtexample
