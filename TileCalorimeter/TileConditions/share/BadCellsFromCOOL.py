# File:     TileConditions/TileInfoDump_jobOptions.py
# Author:   Nils Gollub <nils.gollub@cern.ch>
# Modified: Lukas Pribyl <lukas.pribyl@cern.ch>
#           Henric Wilkens <henric.wilkens@cern.ch>
#__________________________________________________________________

#=============================================================
#===
#=== Usage:
#===  athena.py -c "Folder='OFL01';tileSqlite=True;RUN=178900" BadCellsFromCOOL.py
#===
#=== the default is reading Oracle, to read from sqlite replica, set tileSqlite=True
#=== the default folder is OFL02 (UPD4 tag), valid options for the folder variable
#=== are 'ONL01', 'OFL01', 'OFL02(UPD1)', and 'OFL02(UPD4)' or 'OFL02' which are 
#=== equivalent.
#=== if no RUN is given, the current situation is dumped. 
#===
#=============================================================

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
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-10-00-01"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#=== Set the schema (online/offline)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.isOnline = False

#=============================================================
#=== set global tag
#=============================================================
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("COMCOND-ES1P-001-00")
#conddb.setGlobalTag("COMCOND-REPPST-005-01")

#========================================================
#=== configure TileCoolMgr
#========================================================
from TileConditions.TileCoolMgr import tileCoolMgr

#=== To read /TILE/ONL01/STATUS/ADC from a local tileSqlite.db file uncomment
#

#=== To read /TILE/OFL01/STATUS/ADC from a local tileSqlite.db file uncomment
#tileCoolMgr.setDbConn("oflStatAdc","tileSqlite.db")
#--- change the folder name (OFL01 to OFL02 - this is used in offline schema (Tier0, reprocessing))

if not 'Folder' in dir():
    Folder = 'OFL02'

if not 'tileSqlite' in dir():
    tileSqlite = False
    
#============================================================
#=== select Oracle or sqlite replica
#============================================================
if tileSqlite:
    print 'Setting db connenction to tileSqlite.db'
    if Folder == 'ONL01':
        tileCoolMgr.setDbConn("onlStatAdc","tileSqlite.db")
    else:
        tileCoolMgr.setDbConn("oflStatAdc","tileSqlite.db")
else:
    print 'Setting db connenction to Oracle'
    
#============================================================
#=== select folder to dump
#============================================================
if Folder == 'ONL01':
    tileCoolMgr.setFolder("onlStatAdc","/TILE/ONL01/STATUS/ADC")
    
elif Folder == 'OFL01':
    athenaCommonFlags.isOnline = True
    tileCoolMgr.setFolder("oflStatAdc","/TILE/OFL01/STATUS/ADC")
#    tileCoolMgr.setTag(   "oflStatAdc","HLT-UPD1-00") 

elif Folder == 'OFL02' or Folder == 'OFL02(UPD4)':
    tileCoolMgr.setFolder("oflStatAdc","/TILE/OFL02/STATUS/ADC")
    tileCoolMgr.setTag(   "oflStatAdc","HLT-UPD4-00") 

elif Folder == 'OFL02(UPD1)':
    tileCoolMgr.setFolder("oflStatAdc","/TILE/OFL02/STATUS/ADC")
    tileCoolMgr.setTag(   "oflStatAdc","HLT-UPD1-00") 

else:
    print 'Folder %s not valid' % Folderll
    print 'Use Folder = "ONL01", "OFL01", "OFL02", "OFL02(UPD1)" or "OFL02(UPD4)"'
    print 'Use tile=tileSqlite=True for dumping tileSqlite.db content' 
    exit(2)
    
#=== Setup COOL
from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()
tileInfoConfigurator.setupCOOL()
tileInfoConfigurator.setupCOOLOFC()
tileInfoConfigurator.setupCOOLINTEGRATOR()
tileInfoConfigurator.setupCOOLMUID()

#============================================================
#=== configure TileInfoDump
#============================================================
from TileConditions.TileConditionsConf import TileInfoDump
tileInfoDump = TileInfoDump()
tileInfoDump.AthenaIsOnline=athenaCommonFlags.isOnline()

#=== add private fit method noise tool
from TileConditions.TileCondToolConf import getTileCondToolNoiseRawChn
tileInfoDump.TileCondToolNoiseRawChn = getTileCondToolNoiseRawChn('COOL')

#=== switch section printout on/off
tileInfoDump.PrintBadChannels       = True
tileInfoDump.PrintBadCells          = True

#=== add to the job
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job+= tileInfoDump 

#=== save ROOT histograms and Tuple for bad cells
if hasattr(tileInfoDump,"PrintBadCells"):
    if tileInfoDump.PrintBadCells:
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc()
        svcMgr.THistSvc.Output = [ "AANT DATAFILE='tileBadCells.root' OPT='RECREATE'" ]
        from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
        job += AANTupleStream()
        AANTupleStream = AANTupleStream()
        AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
        AANTupleStream.OutputName = 'tileBadCells.root'
        AANTupleStream.WriteInputDataHeader = True
        AANTupleStream.OutputLevel = WARNING

#============================================================
#=== MessageSvc setup
#============================================================
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 9999999 

#============================================================
#=== Dummy event loop setup
#=== Get the highest run number to display the latest status
#============================================================
from PyCool import cool
if 'RUN' in dir():
    svcMgr.EventSelector.RunNumber     = RUN
else:
    svcMgr.EventSelector.RunNumber     = cool.Int32Max
svcMgr.EventSelector.EventsPerRun      = 5
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 1

#============================================================
#=== print out job summary
#============================================================
print svcMgr
print job



