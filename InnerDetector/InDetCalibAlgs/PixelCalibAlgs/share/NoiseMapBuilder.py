doClusterization = False
doMonitoring     = False
doValidate       = False
filelist         = 'inputfilelist'
nevents          = -1

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc

#------------------------------------------
# Input files
#------------------------------------------
collection = []
if os.path.isfile(filelist):
    for line in open(filelist):
        if line[0] != '#':
            collection.append(line.strip())
else:
    errmess="### Unable to open input filelist: '%s'" % filelist
    raise RuntimeError(errmess)

#------------------------------------------
# DetFlags
#------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

#------------------------------------------
# GlobalFlags
#------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01" # [SGS] how to know exact version (e.g. AMI) ?
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'

# set InputFormat
import PyUtils.AthFile as AthFile
inputfile = AthFile.fopen( collection[0] )
if inputfile.fileinfos['file_type'] == 'bs':
    globalflags.InputFormat = 'bytestream'
elif inputfile.fileinfos['file_type'] == 'pool':
    globalflags.InputFormat = 'pool'
else:
    raise RuntimeError, "Unable to read input file (format not supported)"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = collection

# show debug info [SGS]
print '## globalflags.InputFormat = %s' % globalflags.InputFormat
print '## printing inputfile.fileinfos...'
for k,v in inputfile.fileinfos.iteritems():
    print '  * %s = %s' % (k,v)

if globalflags.InputFormat() == 'pool':
    globalflags.DetDescrVersion = inputfile.fileinfos['geometry']
else:
    globalflags.ConditionsTag = 'CONDBR2-BLKPA-2017-06' 
    globalflags.DatabaseInstance = 'CONDBR2'  

globalflags.print_JobProperties()

#------------------------------------------
# GeoModel
#------------------------------------------
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#------------------------------------------
# ByteStream
#------------------------------------------
if globalflags.InputFormat() == 'pool':
  import AthenaPoolCnvSvc.ReadAthenaPool
  ServiceMgr.EventSelector.InputCollections = collection
  include("PixelConditionsServices/PixelByteStreamErrorsSvc_jobOptions.py") ## why ? 
elif globalflags.InputFormat() == 'bytestream':
    include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
    include("InDetRecExample/InDetReadBS_jobOptions.py")
    ServiceMgr.ByteStreamInputSvc.FullFileName = collection

#------------------------------------------
# Offline Condition
#------------------------------------------
from IOVDbSvc.CondDB import conddb

conddb.addOverride("/PIXEL/NoiseMapLong","PixNoiseMapLong-RUN2-DATA-UPD4-03");
conddb.addOverride("/PIXEL/NoiseMapShort","PixNoiseMapShort-RUN2-DATA-UPD4-03");
conddb.addOverride("/PIXEL/PixMapLong","PixMapLong-RUN2-DATA-UPD1-02");
conddb.addOverride("/PIXEL/PixMapShort","PixMapShort-RUN2-DATA-UPD1-02");

if globalflags.InputFormat() == 'bytestream':
    if len(globalflags.ConditionsTag())!=0:
        conddb.setGlobalTag(globalflags.ConditionsTag())
#conddb.setGlobalTag('CONDBR2-BLKPA-2014-03')  # [SGS] shy different from globalflags.conditionsTag ????

include("PixelConditionsServices/SpecialPixelMapSvc_jobOptions.py")

if not 'doValidate' in dir():
    doValidate=False

if doValidate == False:
    conddb.addOverride('/PIXEL/PixMapShort','PixMapShort-RUN2-DATA-UPD1-02')
    conddb.addOverride('/PIXEL/PixMapLong','PixMapLong-RUN2-DATA-UPD1-02')
else:
    conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=CONDBR2</dbConnection> /PIXEL/NoiseMapShort<tag>PixNoiseMapShort-RUN2-DATA-UPD4-03</tag>" ]
    conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=CONDBR2</dbConnection> /PIXEL/NoiseMapLong<tag>PixNoiseMapLong-RUN2-DATA-UPD4-03</tag>" ]
    ServiceMgr.SpecialPixelMapSvc.DBFolders = [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
    ServiceMgr.SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "NoiseMapLong" ]

from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
ServiceMgr += PixelConditionsSummarySvc()
ServiceMgr.PixelConditionsSummarySvc.UseSpecialPixelMap = False
ServiceMgr.PixelConditionsSummarySvc.UseDCS = False
ServiceMgr.PixelConditionsSummarySvc.UseByteStream = True

#------------------------------------------
# histo service
#------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
if not hasattr(ServiceMgr, 'THistSvc'):
    ServiceMgr += THistSvc()
THistSvc = ServiceMgr.THistSvc
THistSvc.Output += ["histfile DATAFILE='NoiseMap.root' OPT='RECREATE'"]

#------------------------------------------
# NoiseMapBuilder algorithm
#------------------------------------------
from PixelCalibAlgs.PixelCalibAlgsConf import NoiseMapBuilder
NoiseMapBuilder = NoiseMapBuilder()
NoiseMapBuilder.LBMin = 0  
NoiseMapBuilder.LBMax = -1 
print NoiseMapBuilder
topSequence += NoiseMapBuilder

#--------------------------------------------------------------
# events 
#--------------------------------------------------------------
ServiceMgr.EventSelector.SkipEvents = 0
theApp.EvtMax = nevents

#--------------------------------------------------------------
# events and MessageSvc
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel  = INFO
#ServiceMgr.MessageSvc.OutputLevel  = DEBUG
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" 
ServiceMgr.MessageSvc.defaultLimit = 9999999 # all messages 
ServiceMgr.MessageSvc.useColors = True 



