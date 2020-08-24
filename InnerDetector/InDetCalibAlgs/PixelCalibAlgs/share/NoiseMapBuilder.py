## job options for standalone running of NoiseMapBuilder

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

from GaudiSvc.GaudiSvcConf import THistSvc

if not hasattr(ServiceMgr, 'THistSvc'):
    ServiceMgr += THistSvc()

THistSvc = ServiceMgr.THistSvc
THistSvc.Output += ["histfile DATAFILE='NoiseMap.root' OPT='RECREATE'"]


## NoiseMapBuilder algorithm

from PixelCalibAlgs.PixelCalibAlgsConf import NoiseMapBuilder

NoiseMapBuilder = NoiseMapBuilder()

doClusterization = False
doMonitoring = False
doValidate = False


## Input files

collection = []
import os
if os.path.isfile("inputfilelist"):
  for line in open("inputfilelist"):
    collection.append(line.strip())
else:
  raise RuntimeError, "Unable to open inputfilelist"


## GlobalFlags

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'


## input file parameters
from PyUtils.MetaReader import read_metadata
inputfile = read_metadata(collection[0])
inputfile = inputfile[collection[0]]  # promote keys stored under input filename key one level up to access them directly

if inputfile['file_type'] == 'BS':
  globalflags.InputFormat = 'bytestream'
elif inputfile['file_type'] == 'POOL':
  globalflags.InputFormat = 'pool'
else:
  raise RuntimeError, "Unable to read input file (format not supported)"

if inputfile['file_type'] == 'POOL':
  globalflags.DetDescrVersion = inputfile['GeoAtlas']
else:
  globalflags.ConditionsTag = 'CONDBR2-BLKPA-2016-07'  # yosuke
  # globalflags.ConditionsTag = 'CONDBR2-BLKPA-2015-10' # steffen
  globalflags.DatabaseInstance = 'CONDBR2'  ######################
  # globalflags.ConditionsTag = 'OFLCOND-RUN12-SDR-22' #

from IOVDbSvc.CondDB import conddb
conddb.addOverride("/PIXEL/NoiseMapLong","PixNoiseMapLong-RUN2-DATA-UPD4-02")
conddb.addOverride("/PIXEL/NoiseMapShort","PixNoiseMapShort-RUN2-DATA-UPD4-02")
conddb.addOverride("/PIXEL/PixMapLong","PixMapLong-RUN2-DATA-UPD1-02")
conddb.addOverride("/PIXEL/PixMapShort","PixMapShort-RUN2-DATA-UPD1-02")

#conddb.addOverride("/PIXEL/NoiseMapLong","PixNoiseMapLong-DATA-RUN2-000-00")
#conddb.addOverride("/PIXEL/NoiseMapShort","PixNoiseMapShort-DATA-RUN2-000-00")
#conddb.addOverride("/PIXEL/PixMapLong","PixMapLong-DATA-RUN2-000-00")
#conddb.addOverride("/PIXEL/PixMapShort","PixMapShort-DATA-RUN2-000-00")
globalflags.print_JobProperties()


## DetFlags

from AthenaCommon.DetFlags import DetFlags

DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()


## GeoModel

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit



## Offline Conditions

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('CONDBR2-BLKPA-2014-03') ###########
#conddb.blockFolder("/PIXEL/PixCalib");conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib <tag>PixCalib-IBL3D25DBM-04-01</tag>",force=True,forceMC=True);

include("SpecialPixelMapSvc_jobOptions.py")

if 'doValidate' not in dir() :
  doValidate=False

if doValidate is False :
#  conddb.addOverride('/PIXEL/PixMapShort','PixMapShort-DATA-RUN2-000-00')
#  conddb.addOverride('/PIXEL/PixMapLong','PixMapLong-DATA-RUN2-000-00')

  conddb.addOverride('/PIXEL/PixMapShort','PixMapShort-RUN2-DATA-UPD1-02')
  conddb.addOverride('/PIXEL/PixMapLong','PixMapLong-RUN2-DATA-UPD1-02')
else :
#  conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=CONDBR2</dbConnection> /PIXEL/NoiseMapShort<tag>PixNoiseMapShort-DATA-RUN2-000-00</tag>" ]
#  conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=CONDBR2</dbConnection> /PIXEL/NoiseMapLong<tag>PixNoiseMapLong-DATA-RUN2-000-00</tag>" ]

  conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=CONDBR2</dbConnection> /PIXEL/NoiseMapShort<tag>PixNoiseMapShort-RUN2-DATA-UPD4-02</tag>" ]
  conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=CONDBR2</dbConnection> /PIXEL/NoiseMapLong<tag>PixNoiseMapLong-RUN2-DATA-UPD4-02</tag>" ]

  ServiceMgr.SpecialPixelMapSvc.DBFolders = [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
  ServiceMgr.SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "NoiseMapLong" ]

#conddb.setGlobalTag('COMCOND-000-00')
#conddb.setGlobalTag('CONDBR2-BLKPA-2014-03') # 2014-10-28
#conddb.setGlobalTag('OFLCOND-RUN12-SDR-22') # 2014-10-28
#https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CoolProdTags


#
#-- set that if using some ModuleOverlay maks
#
#conddb.iovdbsvc.Folders += [ "<dbConnection>impl=cool;techno=sqlite;schema=module_overlay.db;X:CONDBR2</dbConnection> /PIXEL/PixMapOverlay <tag>PixMapOverlay-Test-00</tag>" ]
#ServiceMgr.SpecialPixelMapSvc.ModuleOverlayFolder = "/PIXEL/PixMapOverlay"
#ServiceMgr.SpecialPixelMapSvc.ModuleOverlayKey = "PixMapOverlay"


from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc

ServiceMgr += PixelConditionsSummarySvc()
ServiceMgr.PixelConditionsSummarySvc.UseSpecialPixelMap = False
ServiceMgr.PixelConditionsSummarySvc.UseDCS = False
ServiceMgr.PixelConditionsSummarySvc.UseByteStream = True


## Input files

if globalflags.InputFormat() == 'pool':
  import AthenaPoolCnvSvc.ReadAthenaPool
  ServiceMgr.EventSelector.InputCollections = collection
  include("PixelByteStreamErrorsSvc_jobOptions.py")
elif globalflags.InputFormat() == 'bytestream':
  include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
  include("InDetRecExample/InDetReadBS_jobOptions.py")
  ServiceMgr.EventSelector.Input = collection


topSequence += NoiseMapBuilder


#
# include clusterization
#

if doClusterization :

  include( "PixelConditionsServices/PixelCalibSvc_jobOptions.py" )

  from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
  topSequence += InDet__PixelClusterization("PixelClusterization")
  import logging
  logger = logging.getLogger( 'PixelCalibAlgs' )
  logger.info(topSequence.PixelClusterization)

#
# include pixel monitoring package
#


#if doMonitoring :
#
#  ## Root file definition
#  if not hasattr(ServiceMgr, 'THistSvc'):
#    from GaudiSvc.GaudiSvcConf import THistSvc
#    ServiceMgr += THistSvc()
#
#  ServiceMgr.THistSvc.Output += [ "GLOBAL DATAFILE='TestMon.root' OPT='RECREATE'"]
#
#
### add an AthenaMonManager algorithm to the list of algorithms to be ran
### AthenaMonManager is the Algorithm that manages many classes inheriting
### from ManagedMonitorToolBase
#
#  from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
#  monMan = AthenaMonManager( "PixelMonManager" )
#
#  from PixelMonitoring.PixelMonitoringConf import *
#  PixelMainsMon=PixelMainMon()
#  ToolSvc += PixelMainsMon
#  monMan.AthenaMonTools += [ PixelMainsMon ]
#  import logging
#  logger = logging.getLogger( 'PixelCalibAlgs' )
#  logger.info(PixelMainsMon)
#
### FileKey must match that given to THistSvc
#  monMan.FileKey = "GLOBAL"
#
### Set global monitoring parameters: see the AthenaMonManager class
### in the Control/AthenaMonitoring package
#  monMan.ManualDataTypeSetup = True
#  monMan.DataType            = "cosmics"
#  monMan.Environment         = "user"
#  monMan.ManualRunLBSetup    = True
#  monMan.Run                 = 1
#  monMan.LumiBlock           = 1
#
### Set pixel monitoring parameters
###Flags for data container types
#  PixelMainsMon.doRDO = True
#  PixelMainsMon.doRODError = True
#  PixelMainsMon.doSpacePoint = False
#  PixelMainsMon.doCluster = True
#  PixelMainsMon.doTrack = False
#  PixelMainsMon.OfflineDoPixelOccupancy = True
#
###Flags for environment types
#  PixelMainsMon.doPitPix = False
#  PixelMainsMon.doCosmics = False
#  PixelMainsMon.doBeam = False
#  PixelMainsMon.doRodSim = False
#  PixelMainsMon.doOffline = True
#
###Names of storegate containers
#  PixelMainsMon.RDOName = "PixelRDOs"
#  PixelMainsMon.RODErrorName = "pixel_error_summary"
#  PixelMainsMon.ClusterName = "PixelClusters"
###Other parameters
#  PixelMainsMon.PitPixMod1 = 1000000 #first digit always1 (keeps leading 0's from being an issue)
#
#  topSequence += monMan
#


#ServiceMgr.EventSelector.SkipEvents = 0
theApp.EvtMax = -1

ServiceMgr.MessageSvc.OutputLevel  = INFO # original
#ServiceMgr.MessageSvc.OutputLevel  = DEBUG # kazuki


topSequence.NoiseMapBuilder.LBMin = 0 #yosuke160415
topSequence.NoiseMapBuilder.LBMax = -1 #yosuke160415
