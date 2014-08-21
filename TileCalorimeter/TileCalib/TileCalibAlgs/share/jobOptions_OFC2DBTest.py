#**************************************************************
#
# jopOptions file to test TileOFC2DBAlg
# File: TileConditions/jobOptions_OFC2DBTest.py
#
#==============================================================

import AthenaCommon.AtlasUnixGeneratorJob
from   AthenaCommon import CfgMgr
from   AthenaCommon.AppMgr import theApp
from   AthenaCommon.AppMgr import ServiceMgr as svcMgr
from   AthenaCommon.AppMgr import ToolSvc

#=============================================================
#=== Global Flags and Geometry setup
#=============================================================
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()    
#--- Geometry setup
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOn()
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


#=============================================================
#=== set global tag
#=============================================================
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-ES1C-000-00')
# conddb.setGlobalTag("COMCOND-006-00")

#========================================================
from TileConditions.TileCoolMgr import tileCoolMgr
#========================================================
from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()
tileInfoConfigurator.setupCOOL()
tileInfoConfigurator.setupCOOLOFC()
tileInfoConfigurator.setupCOOLPHYPULSE()
tileInfoConfigurator.setupCOOLAutoCr()

#============================================================
#=== configure TileCondToolOfc
#============================================================
from TileConditions.TileConditionsConf import TileCondToolOfc
tileCondToolOfc = TileCondToolOfc()
tileCondToolOfc.nSamples = 7 # default = 7
tileCondToolOfc.OptFilterDeltaCorrelation = False # False - use matrix from DB
tileCondToolOfc.CacheSize = 0 # (max phases per channel) 0 - no cache (default)
tileCondToolOfc.OutputLevel = INFO

from TileConditions.TileCondToolConf import *
#                                        ('LAS' or 'PHY' or 'CISPULSE100')
tileCondToolOfc.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','PHY')
# tileCondToolOfc.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','CISPULSE100')

#--------------------------------------------------------------
#  Add OFC2DB Algorithm, options
#--------------------------------------------------------------
ToolSvc += getTileCondToolTiming('COOL','CIS')
theApp.Dlls += [ "TileCalibAlgs" ]

TileOFC2DBAlg = Algorithm("TileOFC2DBAlg")
TileOFC2DBAlg.FixedPhases = True # True - 2001 phases for default channel only
TileOFC2DBAlg.OF2 = True # True - default
TileOFC2DBAlg.RunType = "PHY" # PHY (default) or LAS or CIS
# TileOFC2DBAlg.LbnIOVUntil = 99999
TileOFC2DBAlg.OutputLevel = INFO
theApp.TopAlg = [ "TileOFC2DBAlg" ]


#--------------------------------------------------------------
# svcMgr.IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=tileSqlite.db;DUMMY:OFLP200"
# svcMgr.IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=tileSqlite.db;DUMMY:COMP200"
svcMgr.IOVDbSvc.dbConnection="sqlite://;schema=tileSqlite.db;dbname=COMP200"

#--------------------------------------------------------------
# Options for IOVRegistrationSvc
import RegistrationServices.IOVRegistrationSvc
regSvc = svcMgr.IOVRegistrationSvc
regSvc.RecreateFolders = False #NGO True
regSvc.SVFolder = True
regSvc.OverrideNames+=['TileCalibBlobOfc']
regSvc.OverrideTypes+=['Blob16M']


#=== add to the job
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
###AA job+= tileInfoDump 

#============================================================
#=== MessageSvc setup
#============================================================
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

#============================================================
#=== Dummy event loop setup
#============================================================
svcMgr.EventSelector.RunNumber         = 200000
svcMgr.EventSelector.EventsPerRun      = 5
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 5
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 1

#============================================================
#=== print out job summary
#============================================================
print svcMgr
print job
