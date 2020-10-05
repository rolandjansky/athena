#
# set inFileName and outFileName to convert TProfile2D classification to pool
#
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
import IOVDbSvc.IOVDb


if not 'inFileNameC' in dir():
  inFileNameC="LCWeights_h6prod.100004.piplusminuszero_logE.qgspbert.jid000040-50_v3.Phi5._sum.classify.root"
  #inFileNameC="LCWeights_h6prod.100004.piplusminuszero_logE.ftfpbert.jid000054-69_v3._sum.classify.root"

if not 'inFileNameW' in dir():
  inFileNameW="LCWeights_h6prod.100004.piplusminus_logE.qgspbert.jid000040-50_v3.Phi5._sum.nonInv.weights.root"
  #inFileNameW="LCWeights_h6prod.100004.piplusminus_logE.ftfpbert.jid000054-69_v3._sum.nonInv.weights.root"

if not 'inFileNameO' in dir():
  inFileNameO="LCWeights_h6prod.100004.piplusminus_logE.qgspbert.jid000040-50_v3.Phi5._sum.ooc.root"
  #inFileNameO="LCWeights_h6prod.100004.piplusminus_logE.ftfpbert.jid000054-69_v3._sum.ooc.root"

if not 'inFileNameOPi0' in dir():
  inFileNameOPi0="LCWeights_h6prod.100004.pizero_logE.qgspbert.jid000040-50_v3.Phi5._sum.ooc.root"
  #inFileNameOPi0="LCWeights_h6prod.100004.pizero_logE.ftfpbert.jid000054-69_v3._sum.ooc.root"

if not 'outFileName' in dir():
  outFileName="LCWeights_h6prod.QGSPBERT.100004.vv08.jid000040-50_v3.Phi5.piplusminuszero_logE.classify_weights_ooc_oocPi0.pool.root"
  #outFileName="LCWeights_h6prod.FTFPBERT.vv05.100004.piplusminuszero_logE.classify_weights_ooc_oocPi0.pool.root"


if not os.path.exists(inFileNameC):
  exit("No file "+inFileNameW)
if not os.path.exists(inFileNameW):
  exit("No file "+inFileNameW)
if not os.path.exists(inFileNameO):
  exit("No file "+inFileNameO)
if not os.path.exists(inFileNameOPi0):
  exit("No file "+inFileNameOPi0)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloLocalHadCalib.CaloLocalHadCalibConf import CaloReadLCClassificationFile, CaloReadLCWeightsFile, CaloReadLCOutOfClusterFile
ReadLCClassification  = CaloReadLCClassificationFile("ReadLCClassification")
ReadLCWeights         = CaloReadLCWeightsFile("ReadLCWeights")
ReadLCOutOfCluster    = CaloReadLCOutOfClusterFile("ReadLCOutOfCluster")
ReadLCOutOfClusterPi0 = CaloReadLCOutOfClusterFile("ReadLCOutOfClusterPi0")

ReadLCClassification.LCClassificationFileName = inFileNameC
ReadLCClassification.ClassificationKey="EMFracClassify"
topSequence += ReadLCClassification

ReadLCWeights.LCWeightFileName = inFileNameW
ReadLCWeights.CorrectionKey="H1ClusterCellWeights"
topSequence += ReadLCWeights

ReadLCOutOfCluster.LCOutOfClusterFileName = inFileNameO
ReadLCOutOfCluster.CorrectionKey="OOCCorrection"
topSequence += ReadLCOutOfCluster

ReadLCOutOfClusterPi0.LCOutOfClusterFileName = inFileNameOPi0
ReadLCOutOfClusterPi0.CorrectionKey="OOCPi0Correction"
topSequence += ReadLCOutOfClusterPi0

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("myOCA",outFileName)
# List of objects container type#key#foldername
myOCA.ObjectList = [
    "CaloLocalHadCoeff#EMFracClassify#/CALO/HadCalibration2/CaloEMFrac",
    "CaloLocalHadCoeff#H1ClusterCellWeights#/CALO/HadCalibration2/H1ClusterCellWeights",
    "CaloLocalHadCoeff#OOCCorrection#/CALO/HadCalibration2/CaloOutOfCluster",
    "CaloLocalHadCoeff#OOCPi0Correction#/CALO/HadCalibration2/CaloOutOfClusterPi0"
    ]
myOCA.WriteIOV   = True
#myOCA.IOVTagList = [
    #"CaloEMFrac2-H6-2004-01-FTFP-BERT",
    #"CaloH1CellWeights2-H6-2004-01-FTFP-BERT",
    #"CaloHadOOCCorr2-H6-2004-01-FTFP-BERT",
    #"CaloHadOOCCorrPi02-H6-2004-01-FTFP-BERT"
    #] 
#myOCA.IOVTagList = [
    #"CaloEMFrac2-H6-2004-01-QGSP-BERT",
    #"CaloH1CellWeights2-H6-2004-01-QGSP-BERT",
    #"CaloHadOOCCorr2-H6-2004-01-QGSP-BERT",
    #"CaloHadOOCCorrPi02-H6-2004-01-QGSP-BERT"
    #] 
#myOCA.IOVTagList = [
    #"CaloEMFrac2-GEO08-QGSP-BERT-NClus",
    #"CaloH1CellWeights2-GEO08-QGSP-BERT-NClus",
    #"CaloHadOOCCorr2-GEO08-QGSP-BERT-ISol-NClus",
    #"CaloHadOOCCorrPi02-GEO08-QGSP-BERT-ISol-NClus"
    #] 
#myOCA.IOVTagList = [
#    "CaloEMFrac2-GEO08-FTFP-BERT-NClus",
#    "CaloH1CellWeights2-GEO08-FTFP-BERT-NClus",
#    "CaloHadOOCCorr2-GEO08-FTFP-BERT-NClus",
#    "CaloHadOOCCorrPi02-GEO08-FTFP-BERT-NClus"
#    ] 
#myOCA.IOVTagList = [
    #"CaloEMFrac2-GEO16-QGSP-BERT-Side",
    #"CaloH1CellWeights2-GEO16-QGSP-BERT-Side",
    #"CaloHadOOCCorr2-GEO16-QGSP-BERT-Side",
    #"CaloHadOOCCorrPi02-GEO16-QGSP-BERT-Side"
    #] 
myOCA.IOVTagList = [
    "CaloEMFrac2-GEO180100-QGSP-BERT",
    "CaloH1CellWeights2-GEO180100-QGSP-BERT",
    "CaloHadOOCCorr2-GEO180100-QGSP-BERT",
    "CaloHadOOCCorrPi02-GEO180100-QGSP-BERT"
    ] 
myOCA.OutputLevel= Lvl.DEBUG

AthenaPoolCnvSvc.PoolAttributes += [ "STREAM_MEMBER_WISE = '0'" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = Lvl.INFO

DetectorStore = svcMgr.DetectorStore
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = Lvl.INFO

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel = Lvl.INFO
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myH60401FTFPBERT200.db;dbname=OFLP200"
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myH60401QGSPBERT200.db;dbname=OFLP200"
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo08QGSPBERTNClusIsol200.db;dbname=OFLP200"
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo08FTFPBERTNClus200.db;dbname=OFLP200"
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo16QGSPBERT200.db;dbname=OFLP200"
svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo18QGSPBERT200.db;dbname=OFLP200"
