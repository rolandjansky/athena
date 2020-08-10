from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.BCM_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.pixel_setOn()
DetFlags.TRT_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

DetDescrVersion = 'ATLAS-P2-ITK-22-02-00'

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion

## Need to set the layout option up front
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

SLHC_Flags.doGMX.set_Value_and_Lock(True)
include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")

from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.AtlasVersion = globalflags.DetDescrVersion()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += GeoModelSvc

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-MC15c-SDR-14-03")

from InDetDetDescrExample.InDetDetDescrExampleConf import ReadSiDetectorElements

ReadStripElements = ReadSiDetectorElements("ReadStripElements")
ReadStripElements.UseSiProperties = False
ReadStripElements.LoopOverElements = False
ReadStripElements.TestNegativeStrips = False
ReadStripElements.ManagerName = "SCT"
job += ReadStripElements;

theApp.EvtMax = 1

include("InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py")
