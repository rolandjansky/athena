#import AthenaCommon.AtlasUnixStandardJob
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_geant4()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.detdescr.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#setup GeoModel
DetDescrVersion="ATLAS-GEO-03-00-00"
#DetDescrVersion="ATLAS-GEO-02-01-00"
#DetDescrVersion="ATLAS-CSC-01-00-00"
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#include( "CaloCondAthenaPool/CaloTTMap_Pool_Read.py" )

# ............ old style jO
# --------------------------------------------------------
#load relevant libraries
#theApp.Dlls += [ "TestLArHardwareID" ]
#theApp.TopAlg += [ "reinitTTMap_Algo"]
#reinitTTMap_Algo = Algorithm( "reinitTTMap_Algo" )
#reinitTTMap_Algo.dumpMap = False

# ............ new style jO
# --------------------------------------------------------
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from TestLArHardwareID.TestLArHardwareIDConf import reinitTTMap_Algo
topSequence += reinitTTMap_Algo( "reinitTTMap_Algo" )
#reinitTTMap_Algo.dumpMap = False


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.IOVDbSvc.GlobalTag="OFLCOND-CSC-01-00-00"
# input map
#--------------------------------------------------------------
# Set input file for CondProxyProvider to find the conditions
#--------------------------------------------------------------
from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
svcMgr += getConfigurable( "CondProxyProvider" )()
#svcMgr.CondProxyProvider.InputCollections = [ "LArTTCellMap-DC3-05.pool.root" ]
svcMgr.CondProxyProvider.InputCollections +=["LFN:oflcond.000002.conditions.simul.pool.v0000._0048.pool.root"]
#svcMgr.CondProxyProvider.InputCollections +=["LFN:oflcond.000002.conditions.simul.pool.v0000._0047.pool.root"]



# Load POOL support
import AthenaPoolCnvSvc.WriteAthenaPool

theApp.OutStream = []

#from AthenaCommon.AppMgr import ToolSvc
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
ToolSvc += AthenaPoolOutputStreamTool("CondStream1", OutputFile = "LArTTCellMap-HadFcalFix.pool.root")

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit      = 200000
MessageSvc.useColors   = TRUE

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber=1
svcMgr.EventSelector.FirstEvent=1




