
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import ToolSvc

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

# Pool input
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ 
    "/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00001.pool.root.1"
]

# Detector Initialization Initialization
DetDescrVersion = "ATLAS-CSC-01-02-00"
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

# decoding RDO into digits
include( "MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py" )

include( "MuonRdoToPrepData/MuonRdoToMuonPrepData_jobOptions.py" )

MessageSvc.OutputLevel       = DEBUG
ServiceMgr.StoreGateSvc.Dump = False  #true will dump data store contents
theApp.EvtMax = 200

