#======================================================================
# Offline Muon Monitoring configuration
#======================================================================
# Standalone JobOptions to run MuonDQAMonitoring on ESD/AOD
#======================================================================  

# Set up the reading of the input ESD or xAOD:
FNAME = '/afs/cern.ch/user/g/gcree/work/public/MuonData/data15_cos.00252404.physics_IDCosmic.merge.AOD.x294_m1374/data15_cos.00252404.physics_IDCosmic.merge.AOD.x294_m1374._lb0002-lb0006._0001.1'
include( "AthenaPython/iread_file.py" )
# import AthenaPoolCnvSvc.ReadAthenaPool
# ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "MyMuonMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "cosmics"
#monMan.DataType            = "monteCarlo"
#monMan.DataType            = "collisions"
monMan.Environment         = "tier0ESD"
# monMan.ManualRunLBSetup    = True
# monMan.Run                 = 1
# monMan.LumiBlock           = 1
monMan.FileKey = "MuonMon"
topSequence += monMan

#from MuonRecExample.MuonRecFlags import muonRecFlags

from AthenaMonitoring.DQMonFlags import DQMonFlags  
DQMonFlags.doMuonRawMon=False
DQMonFlags.doMuonSegmentMon=False
DQMonFlags.doMuonTrackMon=True
DQMonFlags.doMuonAlignMon=False
DQMonFlags.doMuonTrkPhysMon=False

DQMonFlags.monManFileKey="MuonMon"
#include ("AthenaMonitoring/DataQualitySteering_jobOptions.py")

from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
MuonDQADetFlags.doMuonMonitoring = True
# --- MuonPhysics Monitoring options
MuonDQADetFlags.doMuonPhysMon = False
MuonDQADetFlags.doMuTrkMon = False 
MuonDQADetFlags.doZmumuMon = False

muonOutputLevel=INFO
include ("MuonTrackMonitoring/MuonTrackDQA_options.py")

for tool in monMan.AthenaMonTools:
    tool.EnableLumi = False
    
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["MuonMon DATAFILE='muonMon.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
