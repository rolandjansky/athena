from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import ToolSvc

# the Athena Pool converters
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

# Detector Initialization Initialization
DetDescrVersion = "ATLAS-GEO-02-00-00"
#DetDescrVersion = "ATLAS-CSC-01-02-00"
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-06')
#conddb.setGlobalTag('OFLCOND-CSC-01-00-00')  

include ("CscCalibTools/CscCalibTool_jobOptions.py")
ToolSvc.CscCalibTool.ReadFromDatabase = True

#top algorithms to be run
include("CSC_Digitization/CSC_Digitization_jobOptions.py")
#CscDigitBuilder.noiseLevel = 0.0
#CscDigitBuilder.pedestal = 0
CscDigitBuilder.OutputLevel = INFO
include( "MuonByteStreamCnvTest/CscDigitToCscRDO_jobOptions.py" )
from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import CscDigitToCscRDO
CscDigitToCscRDO.OutputLevel = DEBUG

include("MuonRdoToPrepData/CscRdoToCscPrepData_jobOptions.py")
from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
CscRdoToCscPrepData.OutputLevel = INFO #DEBUG
from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
CscRdoToCscPrepData.RegSel_CSC = makeRegSelTool_CSC()

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
# Pool input
#ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00001.pool.root.1" ]
#ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root" ]  
ServiceMgr.EventSelector.InputCollections = [
#    "/afs/cern.ch/user/k/ketevi/w0/data/misal1_csc11.005850.WH120bb_pythia.simul.HITS.v12003103_tid003856._00001.pool.root.5",
    '/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110005.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110009.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110012.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110030.pool.root.2'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110039.pool.root.2'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110050.pool.root.2'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110105.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110129.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110137.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110140.pool.root.2'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110166.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110167.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110187.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110191.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110194.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110196.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110209.pool.root.2'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110219.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110237.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110251.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110257.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110258.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110266.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110267.pool.root.1'
    ,'/raid01/DATA/HIT/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462_tid0722691/HITS.072269._110271.pool.root.1'
#   '/raid01/DATA/HIT/mu_E100GeV_GEO-02-00-00.simHITS._0001.pool.root',
#   '/raid01/DATA/HIT/mu_E100GeV_GEO-02-00-00.simHITS._0002.pool.root'
    ]

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

ServiceMgr.StoreGateSvc.Dump = False  #true will dump data store contents
theApp.EvtMax = 1000000

# write to POOL
#include ("MuonEventAthenaPool/MuonRdoToPool_jobOptions.py")
