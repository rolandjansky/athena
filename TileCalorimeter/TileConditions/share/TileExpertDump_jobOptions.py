# File:     TileConditions/TileExpertDump_jobOptions.py
# Author:   Nils Gollub <nils.gollub@cern.ch>
# Modified: Lukas Pribyl <lukas.pribyl@cern.ch>
# Usage:    athena.py -c 'RUN=99000' TileExpertDump_jobOptions.py 
#
# RUN : runnumber

#=== get user options or set default
if not 'RUN' in dir():
    RUN = 999999

#__________________________________________________________________
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
jobproperties.Global.DetDescrVersion = "ATLAS-CommNF-07-00-00"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


#=============================================================
#=== set global tag
#=============================================================
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("COMCOND-HLTS-000-00")

#=============================================================
#=== setup TileCablingSvc
#=============================================================
#svcMgr += CfgMgr.TileCablingSvc()

#========================================================
#=== configure TileCoolMgr
#===
#=== Allows the modification of some global paramters
#=== steering the COOL access for Tile.
#===
#========================================================
from TileConditions.TileCoolMgr import tileCoolMgr

#=== To read /TILE/OFL01/CALIB/CIS/FIT/LIN from a local tileSqlite.db file
#=== uncomment
#tileCoolMgr.setTag(   "oflCisFitLin","COM-00")
#tileCoolMgr.setDbConn("oflCisFitLin","tileSqlite.db")
#tileCoolMgr.setDbConn("onlCisLin","tileSqlite.db")
#tileCoolMgr.setTag(   "oflCisFitNln","HLT-UPD1-00")
#tileCoolMgr.setDbConn("oflCisFitNln","tileSqlite.db")

#=== CALIB/EMS
#tileCoolMgr.setTag(   "oflEms","HLT-01")
#tileCoolMgr.setDbConn("oflEms","tileSqlite.db")
#tileCoolMgr.setDbConn("onlEms","tileSqlite.db")

#=== /TILE/OFL01/CALIB/CES
#tileCoolMgr.setDbConn("onlCes","sqlite://x;schema=tileSqlite.db.def;dbname=COMP200")
#tileCoolMgr.setTag(   "oflCes","HLT-UPD1-01")
#tileCoolMgr.setDbConn("oflCes","tileSqlite.db")

#=== /TILE/OFL01/CALIB/LAS/FIBER
#tileCoolMgr.setTag(   "oflLasFib","COM-00")
#tileCoolMgr.setDbConn("oflLasFib","tileSqlite.db")

#=== To read /TILE/ONL01/CALIB/LAS/LIN from a local tileSqlite.db file
#=== uncomment (no tag is set for ONL01 folders):
#tileCoolMgr.setDbConn("onlLasLin","tileSqlite.db")

#=== If you wish to read another folder from your local tileSqlite.db file
#=== and you need to know the condID name of this folder, please check
#=== TileConditions/python/TileCoolMgr.py

#=== Setup COOL
from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()
tileInfoConfigurator.setupCOOLEMEXPERT()

#============================================================
#=== configure TileExpertDump
#============================================================
from TileConditions.TileConditionsConf import TileExpertDump
tileExpertDump = TileExpertDump()

#=== switch section printout on/off
tileExpertDump.PrintExpertEmscale           = True # set the expert em scale options below
#tileExpertDump.PrintExpertEmscaleOnl        = True # set the expert em scale options below

#=== Expert Emscale Options (used only for PrintExpertEmscale and PrintExpertEmscaleOnl

tileExpertDump.useOflCisFitLin  = True
tileExpertDump.useOflCisFitNln  = True
tileExpertDump.useOflLasLin     = True
tileExpertDump.useOflLasNln     = True
tileExpertDump.useOflLasPart    = True
tileExpertDump.useOflCes        = True
tileExpertDump.useOflEms        = True
tileExpertDump.useOflLasLinPisa = True
tileExpertDump.useOflLasNlnPisa = True
tileExpertDump.useOflMinbias    = True
tileExpertDump.useOnlCisLin     = True
tileExpertDump.useOnlLasLin     = True
tileExpertDump.useOnlCes        = True
tileExpertDump.useOnlEms        = True

#=== add to the job
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job+= tileExpertDump 

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
