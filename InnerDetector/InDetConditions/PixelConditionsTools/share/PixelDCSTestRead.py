import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True


from AthenaCommon.GlobalFlags import GlobalFlags
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
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()

# ---- switch parts of ID off/on as follows
#switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()


#DetDescrVersion = "ATLAS-DC3-05"

#include ("AtlasGeoModel/SetGeometryVersion.py")
import AtlasGeoModel.SetGeometryVersion
#include ("AtlasGeoModel/GeoModelInit.py")
import AtlasGeoModel.GeoModelInit


from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc


from PixelConditionsTools.PixelConditionsToolsConf import PixelDCSTool


ToolSvc += PixelDCSTool(
                            #OutputLevel = DEBUG,
                            RegisterCallback = True,
                            TemperatureFolder = "/PIXEL/DCS/TEMPERATURE",
                            HVFolder = "/PIXEL/DCS/HV",
                            FSMStatusFolder = "/PIXEL/DCS/FSMSTATUS",
                            FSMStateFolder = "/PIXEL/DCS/FSMSTATE",
                            TemperatureFieldName = "temperature",
                            HVFieldName = "HV",
                            FSMStatusFieldName = "FSM_status",
                            FSMStateFieldName = "FSM_state",
                            WriteDefault = False,
                            Temperature = True,
                            HV = True,
                            FSMStatus = True,
                            FSMState = True
                            )



from PixelConditionsTools.PixelConditionsToolsConf import PixelDCSTestReadWrite

job += PixelDCSTestReadWrite(Write = False)



## from IOVDbSvc.IOVDbSvcConf import IOVDbSvc

## MYIOVDbSvc = IOVDbSvc(dbConnection = "impl=cool;techno=sqlite;schema=mytest.db;X:TESTCOOL")
## #MYIOVDbSvc = IOVDbSvc(dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_AAD;DEVDB10:DCSTEST5:ATLAS_COOL_AAD")
## MYIOVDbSvc.Folders += [ "/Test/PixelDCS1 <tag>test_temperature</tag>", "/Test/PixelDCS2 <tag>test_HV</tag>",
##                         "/Test/PixelDCS3 <tag>test_FSMStatus</tag>", "/Test/PixelDCS4 <tag>test_FSMState</tag>" ]

## #MYIOVDbSvc.Folders += [ "/Test/PixelDCS2 <tag>test_HV</tag>", "/Test/PixelDCS3 <tag>test_FSM</tag>" ]
## ServiceMgr += MYIOVDbSvc




from IOVDbSvc.CondDB import conddb

conddb.addFolder("","<dbConnection>impl=cool;techno=sqlite;schema=mytest.db;X:OFLP200</dbConnection> /PIXEL/DCS/TEMPERATURE <tag>PixDCSTemperature-00-00-00</tag>")
conddb.addFolder("","<dbConnection>impl=cool;techno=sqlite;schema=mytest.db;X:OFLP200</dbConnection> /PIXEL/DCS/HV <tag>PixDCSHV-00-00-00</tag>")
conddb.addFolder("","<dbConnection>impl=cool;techno=sqlite;schema=mytest.db;X:OFLP200</dbConnection> /PIXEL/DCS/FSMSTATUS <tag>PixDCSFSMStatus-00-00-00</tag>")
conddb.addFolder("","<dbConnection>impl=cool;techno=sqlite;schema=mytest.db;X:OFLP200</dbConnection> /PIXEL/DCS/FSMSTATE <tag>PixDCSFSMState-00-00-00</tag>")


## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:OFLP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/TEMPERATURE <tag>PixDCSTemperature-00-00-00</tag>")
## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:OFLP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/HV <tag>PixDCSHV-00-00-00</tag>")
## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:OFLP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/FSMSTATUS <tag>PixDCSFSMStatus-00-00-00</tag>")
## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:OFLP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/FSMSTATE <tag>PixDCSFSMState-00-00-00</tag>")

## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:COMP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/TEMPERATURE ")
## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:COMP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/HV ")
## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:COMP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/FSMSTATUS ")
## conddb.addFolder("","<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLOFL_DCS;ATLAS_COOLPROD:COMP200:ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/FSMSTATE ")


from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc.GlobalTag = "DEFAULTCOND"

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.RunNumber         = 0
svcMgr.EventSelector.EventsPerRun      = 20
svcMgr.EventSelector.FirstEvent        = 1
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 10
svcMgr.EventSelector.FirstLB           = 0
svcMgr.EventSelector.EventsPerLB       = 2


# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = 3
svcMgr.MessageSvc.debugLimit       = 10000
svcMgr.MessageSvc.infoLimit        = 10000
svcMgr.MessageSvc.errorLimit       = 1000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"



theApp.EvtMax                   = 1


#python /afs/cern.ch/atlas/project/database/CoolTools/PyCoolConsole.py  "oracle://DEVDB10;schema=ATLAS_COOL_AAD;dbname=DCSTEST"
#AtlCoolConsole.py "sqlite://X;schema=mytest.db;dbname=TESTCOOL"

#python /afs/cern.ch/atlas/project/database/CoolTools/PyCoolConsole.py  "oracle://DEVDB10;schema=ATLAS_COOL_AAD;dbname=DCSTEST"
#AtlCoolConsole.py "oracle://DEVDB10;schema=ATLAS_COOL_AAD;dbname=DCSTEST"

#AtlCoolConsole.py "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
