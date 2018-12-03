# File:     TileCalibAlgs/jobOptions_TileInfoDump.py
# Author:   Nils Gollub <nils.gollub@cern.ch>
# Modified: Lukas Pribyl <lukas.pribyl@cern.ch>
# Modified: Andrei Artamonov <andrei.artamonov@cern.ch>
# Usage:    athena.py -c 'RUN=999999' jobOptions_TileInfoDump.py 
#
# RUN : runnumber

#=== get user options or set default
if not 'RUN' in dir():
    RUN = 999999
RunNumber = RUN

if not 'RUN2' in dir(): 
    RUN2 = (RUN>=222222)

#__________________________________________________________________
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

TileUseDCS = False

#--- Geometry setup
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOn()
from AthenaCommon.JobProperties import jobproperties

#--- see https://atlas-geometry-db.web.cern.ch/atlas-geometry-db/
#--- for the geometry updates
if RUN2: jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"
else:    jobproperties.Global.DetDescrVersion = "ATLAS-R1-2012-03-01-00"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#=============================================================
#=== set global tag
#=============================================================
from IOVDbSvc.CondDB import conddb
if RUN2: conddb.setGlobalTag("CONDBR2-BLKPA-2017-15")
else:    conddb.setGlobalTag("COMCOND-BLKPA-RUN1-06")

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


#=== To read /TILE/OFL02/CALIB/CES from a local tileSqlite.db file
#                       note, folder name and COOL source can be used
#tileCoolMgr.setFolder("oflCes","/TILE/OFL02/CALIB/CES")
#tileCoolMgr.setTag(   "oflCes","RUN2-HLT-UPD1-01")
#tileCoolMgr.setDbConn("oflCes","tileSqlite.db")
##tileCoolMgr.setDbConn("oflCes","TILE") #_OFL")

#=== To read /TILE/OFL02/STATUS/ADC from a local tileSqlite.db file uncomment
#tileCoolMgr.setTag(   "oflStatAdc","COM-04")
#tileCoolMgr.setDbConn("oflStatAdc","tileSqlite.db")

#=== To read /TILE/OFL02/CALIB/CIS/LIN from a local tileSqlite.db file
#=== uncomment
#tileCoolMgr.setTag(   "oflCisLin","COM-00")
#tileCoolMgr.setTag(   "oflCisLin","RUN2-HLT-UPD1-00")
#tileCoolMgr.setDbConn("oflCisLin","tileSqlite.db")

#=== To read /TILE/OFL02/NOISE/SAMPLE from a local tileSqlite.db file
#tileCoolMgr.setTag(   "oflNoiseAdc","COM-01")
#tileCoolMgr.setDbConn("oflNoiseAdc","tileSqlite.db")

#=== To read /TILE/OFL02/NOISE/FIT from a local tileSqlite.db file
#tileCoolMgr.setTag(   "oflNoiseFit","COM-01")
#tileCoolMgr.setDbConn("oflNoiseFit","tileSqlite.db")

#=== To read /TILE/OFL02/INTEGRATOR from a local tileSqlite.db file
tileCoolMgr.setFolder('oflIntGain','/TILE/OFL02/INTEGRATOR')
tileCoolMgr.setTag(   "oflIntGain","RUN2-HLT-UPD1-00")
#tileCoolMgr.setDbConn("oflIntGain","tileSqlite.db")

#=== To read /TILE/OFL02/TIME/CHANNELOFFSET/CIS from a local tileSqlite.db file
#tileCoolMgr.setTag(   "oflTimeCphy","RUN2-HLT-UPD1-00")
#tileCoolMgr.setDbConn("oflTimeCphy","tileSqlite.db")

#=== To read /TILE/ONL01/CALIB/LAS/LIN from a local tileSqlite.db file
#=== uncomment (no tag is set for ONL01 folders):
#tileCoolMgr.setDbConn("onlLasLin","tileSqlite.db")

#=== To read /TILE/ONL01/FILTER/OF2/LAS from a local tileSqlite.db file
#=== uncomment (no tag is set for ONL01 folders):
# tileCoolMgr.setDbConn("onlOfcLas","tileSqlite.db")

#=== If you wish to read another folder from your local tileSqlite.db file
#=== and you need to know the condID name of this folder, please check
#=== TileConditions/python/TileCoolMgr.py

#=== Setup COOL
include( "TileConditions/TileConditions_jobOptions.py" )
tileInfoConfigurator.setupCOOL()
tileInfoConfigurator.setupCOOLOFC()
tileInfoConfigurator.setupCOOLINTEGRATOR()
tileInfoConfigurator.setupCOOLMUID()
tileInfoConfigurator.setupCOOLPHYPULSE()
tileInfoConfigurator.setupCOOLAutoCr()

#============================================================
#=== configure TileInfoDump
#============================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()
from TileCalibAlgs.TileCalibAlgsConf import TileInfoDump
tileInfoDump = TileInfoDump()
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
tileInfoDump.AthenaIsOnline=athenaCommonFlags.isOnline()

#=== add private fit method noise tool
from TileConditions.TileCondToolConf import getTileCondToolNoiseRawChn
tileInfoDump.TileCondToolNoiseRawChn = getTileCondToolNoiseRawChn('COOL')

#=== switch section printout on/off
tileInfoDump.PrintEmscale           = True
tileInfoDump.PrintEmscaleOnl        = True
tileInfoDump.PrintLaser             = True
tileInfoDump.PrintIntegrator        = True
tileInfoDump.PrintPedestals         = True
tileInfoDump.PrintTimingCorrections = True  # set also run type below
tileInfoDump.PrintBadChannels       = True
tileInfoDump.PrintPulseShapes       = True  # set also run type below
tileInfoDump.PrintOfcsCool          = True  # set also run type below
tileInfoDump.PrintOfcRos            = 0   
tileInfoDump.PrintOfcDrawer         = 0
tileInfoDump.PrintOfcChannel        = 0
tileInfoDump.PrintBadCells          = True
tileInfoDump.PrintMuID              = False
tileInfoDump.Print1gNoise           = True

#=== in the case of Timing specify one run type (PHY=default, LAS, CIS)
#from TileConditions.TileCondToolConf import *
#tileInfoDump.TileCondToolTiming = getTileCondToolTiming('COOL','PHY')
#tileInfoDump.TileCondToolTiming = getTileCondToolTiming('COOL','LAS')
#tileInfoDump.TileCondToolTiming = getTileCondToolTiming('COOL','CIS')

#=== in the case of Pulse shapes specify one run type (PHY=default, LAS,...)
#tileInfoDump.PrintPulseShapesStep   = 0.5
#from TileConditions.TileCondToolConf import *
#tileInfoDump.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','PHY')
# tileInfoDump.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','LAS')
# tileInfoDump.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','CISPULSE100')
# tileInfoDump.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','CISPULSE5P2')
# tileInfoDump.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','CISLEAK100')
# tileInfoDump.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','CISLEAK5P2')

#=== in the case of OFCs specify one run type (PHY=default, LAS, CISPULSE100, CISPULSE5P2)
#from TileConditions.TileCondToolConf import *
#tileInfoDump.TileCondToolOfcCool = getTileCondToolOfcCool('COOL','PHY')
#tileInfoDump.TileCondToolOfcCool = getTileCondToolOfcCool('COOL','LAS')
#tileInfoDump.TileCondToolOfcCool = getTileCondToolOfcCool('COOL','CISPULSE100')
#tileInfoDump.TileCondToolOfcCool = getTileCondToolOfcCool('COOL','CISPULSE5P2')

#=== testing to use "another" tool for the TileInfoDump
# from TileConditions.TileCondToolEmscaleConf import getTileCondToolEmscale
# ToolSvc += getTileCondToolEmscale('OtherTool');
# tileInfoDump.TileCondToolEmscale = ToolSvc.OtherTool
# ToolSvc.OtherTool.ProxyOflLasLin.Source = 'TileDefault.cisFitLin'

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
