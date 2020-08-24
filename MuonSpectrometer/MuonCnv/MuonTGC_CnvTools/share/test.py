# ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr

# MessageSvc
MessageSvc.OutputLevel = INFO
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

# GlobalFlags
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()
GlobalFlags.Luminosity.set_zero()

# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Muon_setOn()

# Select geometry version
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-08-00-00"

# AtlasGeoModel 
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# MuonGeoModel
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.UseConditionDb = 0
MuonDetectorTool.OutputLevel = INFO

# Setup cabling svc
import MuonCnvExample.MuonCablingConfig

# Input data set
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [
    'rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/willocq/1510/digi/MC8.107233.singlepart_mu100.digi.pool.1.root'
    ]

# The number of events to be processed
theApp.EvtMax = 100

# AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# MuonTGC_CnvTools
from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
TgcRdoToPrepDataTool = Muon__TgcRdoToPrepDataTool("TgcPrepDataProviderTool")
TgcRdoToPrepDataTool.OutputLevel = INFO
ToolSvc += TgcRdoToPrepDataTool
print TgcRdoToPrepDataTool

# TgcRdoToTgcPrepData
from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
TgcRdoToTgcPrepData = TgcRdoToTgcPrepData()
TgcRdoToTgcPrepData.PrintPrepData = True
from RegionSelector.RegSelToolConfig import makeRegSelTool_TGC
TgcRdoToTgcPrepData.RegSel_TGC = makeRegSelTool_TGC()
topSequence += TgcRdoToTgcPrepData
