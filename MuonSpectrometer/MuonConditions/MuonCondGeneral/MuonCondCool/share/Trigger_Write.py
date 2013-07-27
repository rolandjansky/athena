import AthenaCommon.AtlasUnixStandardJob
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

## general conf

#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_commis()


#from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.Muon_setOn()
#DetFlags.detdescr.ID_setOff()
#DetFlags.detdescr.LAr_setOff()
#DetFlags.detdescr.Tile_setOff()
#DetFlags.Print()

### write here the db connection string

# access to db directly
#dbConn="oracle://intr;schema=ATLAS_COOL_RPCDQ;dbname=RPC_DQA;user=ATLAS_COOL_RPCDQA"
# use sqlite file
#dbConn="sqlite://;schema=TRIGGER2.db;dbname=RPC_DQA"
dbConn="oracle://devdb10;schema=ATLAS_RPC;dbname=RPC_DQA;user=ATLAS_RPC"
### write here the folder to use


#specific algos/tools

phifolder="/RPC/TRIGGER/CM_THR_PHI"
etafolder="/RPC/TRIGGER/CM_THR_ETA"
from MuonCondSvc.MuonCondSvcConf import MuonCalib__TriggerCoolSvc
TriggerCoolSvc=MuonCalib__TriggerCoolSvc("TriggerCoolSvc")
TriggerCoolSvc.OutputLevel=VERBOSE
TriggerCoolSvc.EtaFolder=etafolder
TriggerCoolSvc.PhiFolder=phifolder
# specify the COOL datatbase connection string

from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn

ServiceMgr += TriggerCoolSvc

from MuonCondCool.MuonCondCoolConf import MuonCalib__TriggerCool

TriggerCool=MuonCalib__TriggerCool()

TriggerCool.OutputLevel=VERBOSE
TriggerCool.WritePhi=False
TriggerCool.Read=False
TriggerCool.WriteEta=False

TriggerCool.Chan_Phi=7
TriggerCool.FileName_CM_Phi="CM_name"
TriggerCool.FileName_Th0_Phi="fileTh0.dat"
TriggerCool.InfoPhi=" "

TriggerCool.Chan_Eta=7
TriggerCool.FileName_CM="CM_EtaName"
TriggerCool.FileName_Th0="Th0.dat"
# only one file merged
#TriggerCool.FileName_Th1="Th1.dat"
#TriggerCool.FileName_Th2="Th2.dat"
TriggerCool.Sequence="3,4,6"
TriggerCool.InfoEta=" "

# data is written to conditions database using OutputConditionsAlg

if(TriggerCool.WritePhi or TriggerCool.Read ):
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    OutCondAlg = OutputConditionsAlg("OutCondAlg","dummy.root")
    prefix = "CondAttrListCollection#"
    OutCondAlg.ObjectList=[prefix+phifolder]

    OutCondAlg.IOVTagList = ["TestPhi"]
    OutCondAlg.WriteIOV=True
    OutCondAlg.Run1=1000
    #OutputConditionsAlg.Event1=0
    OutCondAlg.Run2=3000
    #OutputConditionsAlg.Event2=9999
    OutCondAlg.OutputLevel=VERBOSE
else:
    ServiceMgr.IOVDbSvc.Folders+=[phifolder+" <tag>TestPhi</tag> <dbConnection>"+dbConn+"</dbConnection>"] 
if(TriggerCool.WriteEta ):
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    OutCondAlg = OutputConditionsAlg("OutCondAlg","dummy.root")
    prefix = "CondAttrListCollection#"
    OutCondAlg.ObjectList=[prefix+etafolder]

    OutCondAlg.IOVTagList = ["TestEta"]
    OutCondAlg.WriteIOV=True
    OutCondAlg.Run1=1
    #OutputConditionsAlg.Event1=0
    OutCondAlg.Run2=30000000
    #OutputConditionsAlg.Event2=9999
    OutCondAlg.OutputLevel=VERBOSE
else:
    ServiceMgr.IOVDbSvc.Folders+=[etafolder+" <tag>TestEta</tag> <dbConnection>"+dbConn+"</dbConnection>"]      

job+=TriggerCool
#ServiceMgr.EventSelector.RunNumber  = 1189
theApp.EvtMax=1

#theApp.initialize()
MessageSvc.OutputLevel = VERBOSE
