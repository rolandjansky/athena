
#Simple jobOptions illustrating using an sqlite file for beamspot positions


# Input files
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.EventSelector.InputCollections = ["aod.pool.root"]
# Number of events to be processed (default is 10)
theApp.EvtMax = -1

ToolSvc = Service('ToolSvc')
from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotReader as InDetBeamSpotReader

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += InDetBeamSpotReader()


from IOVDbSvc.CondDB import conddb
    
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#conddb.Folders+=[ "INDET"   + "/Indet/Beampos" + "<tag>nominal</tag>"]

IOVDbSvc = Service("IOVDbSvc")
# IOVDbSvc.overrideTags += ["<prefix>/Indet/Beampos</prefix> <tag>nominal</tag>"]
# IOVDbSvc.overrideTags += ["<prefix>/Indet/Beampos</prefix>"]
# IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=beamspot.db;X:TESTCOOL"
#IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=beamspot.db;X:BEAMSPOT</dbConnection> /Indet/Beampos<tag>nominal</tag>"]
IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=beamspot.db;dbname=BEAMSPOT</dbConnection> /Indet/Beampos<tag>nominal</tag>"]
