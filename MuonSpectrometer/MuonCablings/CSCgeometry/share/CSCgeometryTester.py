
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 1 #-1 means all events

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr
svcMgr = theApp.serviceMgr()

#Don't know which of these are needed:#######################
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()

include ("RecExCommon/RecExCommon_flags.py")

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()

DetDescrVersion = "ATLAS-CommNF-09-00-00"
include ("RecExCond/AllDet_detDescr.py")

from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit

from IOVDbSvc.CondDB import conddb
#######################################################

include("CSCgeometry/CSCgeometry_jobOptions.py")
from CSCgeometry.CSCgeometryConf import CSCgeometryTester
topSequence += CSCgeometryTester()
CSCgeometryTester = CSCgeometryTester()
#Set parameters like:
#CSCgeometryTester.spork = true
