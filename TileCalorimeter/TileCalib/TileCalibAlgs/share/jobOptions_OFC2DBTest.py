#**************************************************************
#
# jopOptions file to test TileOFC2DBAlg
# File: TileConditions/jobOptions_OFC2DBTest.py
#
#==============================================================
db="tileSqlite_CovMatDefault.db"
RUN2=True
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DatabaseInstance="CONDBR2"

from TileConditions.TileCoolMgr import tileCoolMgr

tileCoolMgr.setFolder("oflNoiseAcr","/TILE/OFL02/NOISE/AUTOCR")
tileCoolMgr.setTag(   "oflNoiseAcr","RUN2-HLT-UPD1-00")
tileCoolMgr.setDbConn("oflNoiseAcr",db)
#=== get user options or set default
if not 'RUN' in dir():
    RUN = 222222

if not 'RUN2' in dir(): 
    RUN2 = (RUN>=222222)

import AthenaCommon.AtlasUnixGeneratorJob
from   AthenaCommon import CfgMgr
from   AthenaCommon.AppMgr import theApp
from   AthenaCommon.AppMgr import ServiceMgr as svcMgr
from   AthenaCommon.AppMgr import ToolSvc

#=============================================================
#=== Global Flags and Geometry setup
#=============================================================
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
if RUN2: globalflags.DatabaseInstance="CONDBR2"
else:    globalflags.DatabaseInstance="COMP200"

#--- Geometry setup
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOn()
from AthenaCommon.JobProperties import jobproperties

#--- see http://atlas.web.cern.ch/Atlas/GROUPS/OPERATIONS/dataBases/DDDB/tag_hierarchy_browser.php
#--- for the geometry updates
if RUN2: jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-02-00-00"
else:    jobproperties.Global.DetDescrVersion = "ATLAS-GEO-20-00-02"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#=============================================================
#=== set global tag
#=============================================================
from IOVDbSvc.CondDB import conddb
if RUN2: conddb.setGlobalTag("CONDBR2-BLKPA-2015-09")
else:    conddb.setGlobalTag("COMCOND-BLKPA-RUN1-06")

#========================================================
#from TileConditions.TileCoolMgr import tileCoolMgr
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
#tileCondToolOfc.OutputLevel = INFO
tileCondToolOfc.OutputLevel = DEBUG

from TileConditions.TileCondToolConf import *
#                                        ('LAS' or 'PHY' or 'CISPULSE100')
tileCondToolOfc.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','PHY')
# tileCondToolOfc.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','CISPULSE100')

#--------------------------------------------------------------
#  Add OFC2DB Algorithm, options
#--------------------------------------------------------------
ToolSvc += getTileCondToolTiming('COOL','CIS')

from TileCalibAlgs.TileCalibAlgsConf import TileOFC2DBAlg
tileOFC2DBAlg = TileOFC2DBAlg()
tileOFC2DBAlg.FixedPhases = True # True - 2001 phases for default channel only
tileOFC2DBAlg.OF2 = False # True - default
tileOFC2DBAlg.RunType = "PHY" # PHY (default) or LAS or CIS
# tileOFC2DBAlg.LbnIOVUntil = 999999999
tileOFC2DBAlg.OutputLevel = INFO

tileOFC2DBAlg.FixedPhasesNumber = 100
tileOFC2DBAlg.PhaseStep = 0.5
tileOFC2DBAlg.Modules = ['AUX01', 'AUX05', 'AUX13', 'AUX14', 'AUX15', 'AUX16', 'AUX17', 'AUX18', 'AUX19', 'AUX20']
tileOFC2DBAlg.CreateAllModules = True # In DB will be stored other missing modules with zeros size (empty)



from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()
topSequence += tileOFC2DBAlg

#--------------------------------------------------------------
# svcMgr.IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=tileSqlite.db;DUMMY:OFLP200"
# svcMgr.IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=tileSqlite.db;DUMMY:COMP200"
svcMgr.IOVDbSvc.dbConnection="sqlite://;schema=tileSqlite.db;dbname=CONDBR2"

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
svcMgr.EventSelector.RunNumber         = RUN
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
