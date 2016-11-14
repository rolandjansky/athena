from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
 
from glob import glob

if ('FILE' in dir()):
    svcMgr.EventSelector.InputCollections=[FILE]
if ('inputDir' in dir()):
    inputFiles = glob(inputDir+'*')
    svcMgr.EventSelector.FilesInput=inputFiles
if ('NOV' in dir()):
     theApp.EvtMax=NOV

cfgtool = CfgMgr.TrigConf__xAODConfigTool("xAODConfigTool")
ToolSvc += cfgtool

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool

ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
## The string "histo" in the argument below is the 'FileKey'
## used by Athena to access the output file internally
svcMgr.THistSvc.Output += ["histo DATAFILE='histo.root' OPT='RECREATE'"]
#svcMgr.THistSvc.PrintAll = True
#svcMgr.THistSvc.OutputLevel = 1

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()

HLTMonManager.FileKey = "histo"

#print HLTMonManager;

