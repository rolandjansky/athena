### Setup Athena common flags
include ("AthenaCommon/AthenaCommonFlags.py")
include.block("AthenaCommon/AthenaCommonFlags.py")

include( "IOVDbSvc/IOVRecExCommon.py" )

doMuonCalibAtlas=True


from AthenaCommon.AppMgr import ServiceMgr
#need to load atlas geometry for the efficiency determination
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data()
GlobalFlags.Print()
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()


from CalibNtupleAnalysisAlg.CalibNtupleAnalysisAlgConf import MuonCalib__MuonCalibGeometryDumper

MuonCalibGeometryDumper = MuonCalib__MuonCalibGeometryDumper()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MuonCalibGeometryDumper
#Set geometry tag
from AthenaCommon.AppMgr import ServiceMgr
from AtlasGeoModel import SetGeometryVersion, GeoModelInit

GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.AtlasVersion = "ATLAS-GEO-16-00-01"


MuonCalibGeometryDumper.RootFile = "geom.root"
theApp.EvtMax = 1
