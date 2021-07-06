from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True


#from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
# --- set defaults
GlobalFlags.DataSource.set_geant4()   
GlobalFlags.InputFormat.set_pool()   
# --- default is zero luminosity
GlobalFlags.Luminosity.set_zero()
GlobalFlags.Print()

#--------------------------------------------------------------
# Set Detector setup

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.SCT_setOff()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()


from AthenaCommon.JobProperties import jobproperties

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True
DetDescrVersion = "ATLAS-GEO-08-00-02"
jobproperties.Global.DetDescrVersion = DetDescrVersion
#DetDescrVersion = GlobalFlags.DetDescrVersion()
GeoModelSvc.AtlasVersion = DetDescrVersion


from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()


import AthenaPoolCnvSvc.ReadAthenaPool


ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch03/Monica2/data09_cos.00136379.physics_IDCosmic.merge.AOD.f160_m213/data09_cos.00136379.physics_IDCosmic.merge.AOD.f160_m213._0001.1"]




#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
#IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3
IOVDbSvc.DBInstance="COOLOFL_DCS"

doMDT_Tube=False
doMDT_Chamber=True
doRPC=True

if do_MDTChamber:
    # Dead Chambers

    from MuonCondSvc.MuonCondSvcConf import MDT_DCSConditionsSvc
    ServiceMgr +=MDT_DCSConditionsSvc()

    from MuonCondTool.MuonCondToolConf import MDT_DCSConditionsTool
    MDT_DCSConditionsTool = MDT_DCSConditionsTool("MDT_DCSConditionsTool")
    MDT_DCSConditionsTool.OutputLevel = VERBOSE 
    #MessageSvc.OutputLevel = DEBUG

    MDT_DCSConditionsTool.DropChamberFolder = "/MDT/DCS/DROPPEDCH"
    MDT_DCSConditionsTool.HVFolder = "/MDT/DCS/PSHVMLSTATE"
    MDT_DCSConditionsTool.LVFolder = "/MDT/DCS/PSLVCHSTATE"
    MDT_DCSConditionsTool.JTAGFolder = "/MDT/DCS/JTAGCHSTATE"
    ToolSvc += MDT_DCSConditionsTool

    dbConn="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
    folder="/MDT/DCS/DROPPEDCH"
    
    
    
    include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
    
    ServiceMgr.IOVDbSvc.dbConnection=dbConn
    ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/DROPPEDCH" +" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
    ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSHVMLSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
    ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSLVCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
    ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/JTAGCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
    from MuonCondSvc.MuonCondSvcConf import MDTCondSummarySvc
    ServiceMgr +=MDTCondSummarySvc()
    
    
    MDTCondSummary=ServiceMgr.MDTCondSummarySvc
    MDTCondSummary.ConditionsServices=["MDT_DCSConditionsSvc"]

    
if doMDT_Tube:    
  
    IOVDbSvc.DBInstance="ATLAS_COOL_MDTDQ"
    dbConn_Mdt="oracle://intr;schema=ATLAS_COOL_MDTDQ;dbname=MDT_DQA"

    folder_Mdt="/CONFIG/DEAD_TUBE"
    
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    from IOVDbSvc.CondDB import conddb
    include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
    
    ServiceMgr.IOVDbSvc.dbConnection=dbConn_Mdt
    ServiceMgr.IOVDbSvc.Folders+=[folder_Mdt+" <tag>HEAD</tag> <dbConnection>"+dbConn_Mdt+"</dbConnection>"]
    
    from MuonCondSvc.MuonCondSvcConf import MDTCondSummarySvc
    ServiceMgr +=MDTCondSummarySvc()
    
    
    MDTCondSummary=ServiceMgr.MDTCondSummarySvc
    MDTCondSummary.ConditionsServices=["MDT_DeadTubeConditionsSvc"]

    
if doMDT_Tube and doMDT_Chamber:
    from MuonCondSvc.MuonCondSvcConf import MDTCondSummarySvc
    ServiceMgr +=MDTCondSummarySvc()
    
    
    MDTCondSummary=ServiceMgr.MDTCondSummarySvc
    MDTCondSummary.ConditionsServices=["MDT_DeadTubeConditionsSvc","MDT_DCSConditionsSvc"]


if doRPC:
    from MuonCondSvc.MuonCondSvcConf import RPCCondSummarySvc
    ServiceMgr +=RPCCondSummarySvc()
    RPCCondSummarySvc=ServiceMgr.RPCCondSummarySvc
    
    
    from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
    RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
    RpcDetectorStatusDbTool.OutputLevel = VERBOSE 
    MessageSvc.OutputLevel = VERBOSE
    RpcDetectorStatusDbTool.RpcDqFolder = "/OFFLINE/FINAL"
    ToolSvc += RpcDetectorStatusDbTool
    
    dbConn_Rpc="oracle://intr;schema=ATLAS_COOL_RPCDQ;dbname=RPC_DQA"

    folder_Rpc="/OFFLINE/FINAL"
    
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    from IOVDbSvc.CondDB import conddb
    include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
    
    ServiceMgr.IOVDbSvc.dbConnection=dbConn_Rpc
    ServiceMgr.IOVDbSvc.Folders+=[folder_Rpc+" <tag>RecoCondDB</tag> <dbConnection>"+dbConn_Rpc+"</dbConnection>"]
    
    

from MuonCondTest.MuonCondTestConf import MDTConditionsTestAlg
job+= MDTConditionsTestAlg()
    
    
theApp.EvtMax                   =  2 

#ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
#ServiceMgr.MessageSvc.OutputLevel = VERBOSE
