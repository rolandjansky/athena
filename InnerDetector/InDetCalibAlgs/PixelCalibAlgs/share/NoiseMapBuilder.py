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

import PyUtils.AthFile as AthFile
inputfile = AthFile.fopen( collection[0] )

if inputfile.fileinfos['file_type'] == 'bs':
  globalflags.InputFormat = 'bytestream'
elif inputfile.fileinfos['file_type'] == 'pool':
  globalflags.InputFormat = 'pool'
else:
  raise RuntimeError, "Unable to read input file (format not supported)"
  

if inputfile.fileinfos['file_type'] == 'pool':
  globalflags.DetDescrVersion = inputfile.fileinfos['geometry']
else:
  globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-02'

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

include("PixelConditionsServices/SpecialPixelMapSvc_jobOptions.py")

if not 'doValidate' in dir() :
  doValidate=False

if doValidate == False :
  conddb.addOverride('/PIXEL/PixMapShort','PixMapShort-empty');
  conddb.addOverride('/PIXEL/PixMapLong','PixMapLong-empty');
else :
  conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=COMP200</dbConnection> /PIXEL/NoiseMapShort<tag>NoiseMapShort-000-00</tag>" ]
  conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=COMP200</dbConnection> /PIXEL/NoiseMapLong<tag>NoiseMapLong-000-00</tag>" ]
  ServiceMgr.SpecialPixelMapSvc.DBFolders = [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
  ServiceMgr.SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "NoiseMapLong" ]

conddb.setGlobalTag('COMCOND-000-00')


#
#-- set that if using some ModuleOverlay maks
#
#conddb.iovdbsvc.Folders += [ "<dbConnection>impl=cool;techno=sqlite;schema=module_overlay.db;X:COMP200</dbConnection> /PIXEL/PixMapOverlay <tag>PixMapOverlay-Test-00</tag>" ]
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
  include("PixelConditionsServices/PixelByteStreamErrorsSvc_jobOptions.py")
elif globalflags.InputFormat() == 'bytestream':
  include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
  include("InDetRecExample/InDetReadBS_jobOptions.py")
  ServiceMgr.ByteStreamInputSvc.FullFileName = collection


topSequence += NoiseMapBuilder


#
# include clusterization
#

if doClusterization :

  include( "PixelConditionsServices/PixelCalibSvc_jobOptions.py" )

  from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
  topSequence += InDet__PixelClusterization("PixelClusterization")
  print topSequence.PixelClusterization

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
#  print PixelMainsMon
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

# include Pixel ntuple writer alg (useful if some further validation is needed)
# adds information about all PRDs in the Pixels
#from InDetTrackValidation.InDetTrackValidationConf import InDet__PixelClusterValidationNtupleWriter
#PixelNtupleWriter = InDet__PixelClusterValidationNtupleWriter(name                       = 'InDetPixelClusterValidationNtupleWriter',
#                                                              NtupleFileName           = 'TRKVAL',
#                                                              NtupleDirectoryName      = 'Validation',
#                                                              NtupleTreeName           = 'PixelRIOs',
#                                                              PixelClusterContainer    = 'PixelClusters')
#topSequence += PixelNtupleWriter
#ServiceMgr.THistSvc.Output += [ "TRKVAL DATAFILE='/tmp/aandreaz/TrkValidation.root' TYPE='ROOT' OPT='RECREATE'" ]



#ServiceMgr.EventSelector.SkipEvents = 0
theApp.EvtMax = -1

ServiceMgr.MessageSvc.OutputLevel  = INFO


