from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import ToolSvc

# the Athena Pool converters
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

# Pool input
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/k/ketevi/w0/data/misal1_csc11.005850.WH120bb_pythia.simul.HITS.v12003103_tid003856._00001.pool.root.5" ]

# Detector Initialization Initialization
DetDescrVersion = "ATLAS-CSC-01-02-00"
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

include( "MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py" )

#top algorithms to be run
include("TGC_Digitization/TGC_Digitization_jobOptions.py")
include( "MuonByteStreamCnvTest/TgcDigitToTgcRDO_jobOptions.py" )
# In case of reading Rome data with new TgcDigit(11.1.0/11.0.2 or later),
# uncomment the follwoing option
#TgcDigitToTgcRDO.isNewTgcDigit = FALSE

ServiceMgr.MessageSvc.OutputLevel      = DEBUG
ServiceMgr.StoreGateSvc.Dump = False  #true will dump data store contents
theApp.EvtMax = 1000

# write to POOL
include ("MuonEventAthenaPool/MuonRdoToPool_jobOptions.py")

