#
# set inFileName and outFileName to convert TProfile2D weights to pool
#
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
import IOVDbSvc.IOVDb

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloLocalHadCalib.CaloLocalHadCalibConf import CaloReadLCWeightsFile
ReadLCWeights = CaloReadLCWeightsFile("ReadLCWeights")

ReadLCWeights.LCWeightFileName = inFileName
ReadLCWeights.CorrectionKey="H1ClusterCellWeights"
topSequence += ReadLCWeights
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("myOCA",outFileName)
# List of objects container type#key#foldername
myOCA.ObjectList = ["CaloLocalHadCoeff#H1ClusterCellWeights#/CALO/HadCalibration2/H1ClusterCellWeights"]
myOCA.WriteIOV   = True
myOCA.IOVTagList = ["CaloH1CellWeights2-GEO08-QGSP-BERT-Log" ] 
myOCA.OutputLevel= Lvl.DEBUG

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = Lvl.INFO

DetectorStore = svcMgr.DetectorStore
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = Lvl.INFO

iovdbsvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel = Lvl.INFO
svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myWeightGeo08BERTLog200.db;dbname=OFLP200"

