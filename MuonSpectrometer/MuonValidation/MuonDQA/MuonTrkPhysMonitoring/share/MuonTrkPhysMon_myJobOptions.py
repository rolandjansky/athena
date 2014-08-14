# $Id: PhysVal_jobOptions.py 590776 2014-04-02 09:02:43Z fthomas $

# Set up the reading of the input xAOD:
FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )
#import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "MuonTrkPhysMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "MuonTrkPhysVal"
topSequence += monMan

from MuonTrkPhysMonitoring.MuonTrkPhysMonitoringConf import MuonTrkPhysMonitoring 

MuonTrkPhysMon_NoTrig = MuonTrkPhysMonitoring(name = "MuonTrkPhysMon_NoTrig",
                                              UseTriggerVector = False,
                                              MuonTriggerChainName = "NoMuonTriggerSelection",
                                              OutputLevel      = DEBUG,
                                              MuonTrackCollections = "Muons",
                                              InnerTrackContainer = "InDetTrackParticles",
                                              isoConeMax        = 0.3,
                                              isoConeMin        = 0.05)

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += MuonTrkPhysMon_NoTrig
monMan.AthenaMonTools += [ MuonTrkPhysMon_NoTrig ]

MuonTrkPhysMon_NoTrig.EnableLumi = False

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["PhysVal DATAFILE='MuonTrkPhysVal.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000





















