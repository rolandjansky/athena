#
# set inFileName and outFileName to convert TProfile2D classification to pool
#
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
import IOVDbSvc.IOVDb

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloLocalHadCalib.CaloLocalHadCalibConf import CaloReadLCClassificationFile
ReadLCClassification = CaloReadLCClassificationFile("ReadLCClassification")

ReadLCClassification.LCClassificationFileName = inFileName
ReadLCClassification.ClassificationKey="EMFracClassify"
topSequence += ReadLCClassification
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("myOCA",outFileName)
# List of objects container type#key#foldername
myOCA.ObjectList = ["CaloLocalHadCoeff#EMFracClassify#/CALO/HadCalibration/CaloEMFrac"]
myOCA.WriteIOV   = True
myOCA.IOVTagList = ["CaloEMFrac2-CSC05-BERT" ] 
myOCA.OutputLevel= Lvl.DEBUG

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = Lvl.INFO

DetectorStore = svcMgr.DetectorStore
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = Lvl.INFO

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel = Lvl.INFO
svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myEMFrac05BERT200.db;dbname=OFLP200"

