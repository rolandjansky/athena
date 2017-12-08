from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

theApp.EvtMax = 250
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
 
if not ('fileList' in dir()):
    fileList=['root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/rel_0/20.7.X.Y-VAL/x86_64-slc6-gcc49-opt/offline/TrigEgammaValidation/RDOtoAOD_MC_transform_Zee_25ns_pileup/AOD.Zee.25ns.pileup.pool.root']

# allow this to work in RTT
if not fileList == []:
    svcMgr.EventSelector.InputCollections=fileList

cfgtool = CfgMgr.TrigConf__xAODConfigTool("xAODConfigTool")
ToolSvc += cfgtool

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool

ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

################ Mon Tools #################

#Global HLTMonTool

from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
HLTMon = HLTMonTool(name               = 'HLTMon',
                   histoPathBase      = "/Trigger/HLT",
                   TrigConfigTool=cfgtool);

ToolSvc += HLTMon;

HLTMonManager.AthenaMonTools += [ "HLTMonTool/HLTMon" ];

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;


