from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

theApp.Dlls+=["LArCalibTest"]
theApp.TopAlg+=["LArReadHadDMCoeffFile2"]
LArReadHadDMCoeff=Algorithm("LArReadHadDMCoeffFile2")
LArReadHadDMCoeff.HadDMCoeffFileName = "/afs/cern.ch/user/p/pospelov/public/HadDMCoeff2/HadDMCoeff2_11122007.txt"
LArReadHadDMCoeff.CorrectionKey="HadDMCoeff2"

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

include("CaloCondAthenaPool/CaloCondAthenaPool_joboptions.py")

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("myOCA","HadDMCoeff2_11122007.pool.root")
#myOCA.ObjectList = ["CaloHadDMCoeff2#HadDMCoeff2"]
myOCA.ObjectList = ["CaloHadDMCoeff2#HadDMCoeff2#/CALO/HadCalibration/CaloDMCorr2"]
myOCA.WriteIOV   = True
myOCA.OutputLevel= DEBUG
myOCA.IOVTagList = [ "CaloHadDMCorr-002-00" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

#DetectorStore = Service( "DetectorStore" )
#DetectorStore.Dump = TRUE
#DetectorStore.OutputLevel = INFO


include( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
IOVDbSvc = Service( "IOVDbSvc" )

IOVDbSvc.dbConnection = "impl=cool;techno=sqlite;schema=myDBHadDMCoeff200.db;X:OFLP200"
