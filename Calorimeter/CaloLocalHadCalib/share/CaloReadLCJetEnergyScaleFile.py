#
# set inFileName and outFileName to convert TProfile2D classification to pool
#
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
import IOVDbSvc.IOVDb

if not 'inFileNames' in dir():
  inFileNames = ["AntiKt4CB_JES_NumInv_JX.root", "AntiKt6CB_JES_NumInv_JX.root" ]

if not 'inJetCollectionNames' in dir():
  inJetCollectionNames = ["AntiKt4LCTopoJets", "AntiKt4LCTopoJets" ]

if not 'outFileName' in dir():
  outFileName = "AntiKt4a6CB_JES_40_NumInv_JX.pool.root"


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloLocalHadCalib.CaloLocalHadCalibConf import CaloReadLCJetEnergyScaleFile
ReadLCJetEnergyScale = CaloReadLCJetEnergyScaleFile("ReadLCJetEnergyScale")

ReadLCJetEnergyScale.LCJetEnergyScaleFileNames = inFileNames
ReadLCJetEnergyScale.LCJetEnergyScaleJetCollectionNames = inJetCollectionNames
ReadLCJetEnergyScale.CorrectionKey="JESCorrection"
topSequence += ReadLCJetEnergyScale
# Number of events to be processed (default is 10)
theApp.EvtMax = 1


AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("myOCA",outFileName)
# List of objects container type#key#foldername
myOCA.ObjectList = ["CaloLocalHadCoeff#JESCorrection#/CALO/HadCalibration2/CaloJetEnergyScale"]
myOCA.WriteIOV   = True
myOCA.IOVTagList = ["CaloHadJESCorr2-GEO18-QGSP-BERT-MU00" ] 
myOCA.OutputLevel= Lvl.DEBUG

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = Lvl.INFO

DetectorStore = svcMgr.DetectorStore
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = Lvl.INFO

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel = Lvl.INFO
svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myJESGEO18BERTMU00200.db;dbname=OFLP200"
