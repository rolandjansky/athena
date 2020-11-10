#**************************************************************
#
# jopOptions file for writing ByteStream. 
#
#==============================================================
#  Read in POOL MC event
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )


from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
DetFlags.Print()


from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
GlobalFlags.InputFormat.set_pool()
GlobalFlags.DetGeo.set_atlas()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-CSC-01-02-00"


# MC Truth
# Pool Converters
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

# Pool input (Change this to use a different file)
#
EventSelector = svcMgr.EventSelector
EventSelector.InputCollections=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]
# need catalog from 
# /afs/cern.ch/atlas/project/dc2/preprod801/PoolFileCatalog.xml 

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include ("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )


# write to ByteStream as eformat file. 
# include( "ByteStreamCnvSvc/WriteByteStream_jobOptions.py" )
#write to ByteStream as EventStorage File.
# include ("ByteStreamCnvSvc/WriteByteStream_EventStorage_jobOptions.py")

include( "LArByteStream/ReadLArBS_jobOptions.py" )

# configure for output 
svcMgr = theApp.serviceMgr()

# Conversion Service instance 
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
# OutputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
svcMgr += ByteStreamEventStorageOutputSvc()
# Properties 
ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
ByteStreamCnvSvc.ByteStreamOutputSvc ="ByteStreamEventStorageOutputSvc"
# OutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStream
oStream = AthenaOutputStream(
    "StreamBS",
    EvtConversionSvc = "ByteStreamCnvSvc",
    OutputFile = "ByteStreamEventStorageOutputSvc"
    )
theApp.addOutputStream( oStream )
theApp.OutStreamType ="AthenaOutputStream"; 
# Define the output as follows: 
ByteStreamEventStorageOutputSvc = svcMgr.ByteStreamEventStorageOutputSvc
ByteStreamEventStorageOutputSvc.OutputDirectory = "./" 
ByteStreamEventStorageOutputSvc.AppName = "Athena" 
ByteStreamEventStorageOutputSvc.FileTag = "csc13" 



# read LArRawChannel from G4, write to BS

theApp.Dlls   += [ "LArCalibUtils" ]
theApp.Dlls   += [ "LArByteStream" ]

oStream.ItemList +=["2721#*"]


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel      = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 50


ByteStreamEventStorageOutputSvc.FileTag = "csc12"
from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig

svcMgr.ToolSvc += LArRawDataContByteStreamToolConfig(InitializeForWriting = True,
                                                     stream = StreamBS)


from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("ChronoAuditor")()
theApp.AuditAlgorithms=True
theApp.AuditServices=True
theApp.AuditTools=True 
