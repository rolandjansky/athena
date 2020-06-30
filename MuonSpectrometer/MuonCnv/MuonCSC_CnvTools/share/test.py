from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# import service manager
from AthenaCommon.AppMgr import ServiceMgr
import AthenaCommon.AtlasUnixStandardJob

######################################################################## setup GeoModel
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()
GlobalFlags.Luminosity.set_zero()


MessageSvc.OutputLevel = INFO
#MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"


# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOn()

# Select geometry version
DetDescrVersion = "ATLAS-CSC-01-02-00"
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel


from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.UseConditionDb = 0
MuonDetectorTool.OutputLevel = INFO
MuonDetectorTool.CachingFlag       = 1 # default is 1
MuonDetectorTool.FillCacheInitTime = 1 # default is 0
#MuonDetectorTool.StationSelection  = 0 # default is 0
#MuonDetectorTool.SelectedStations  = [ "EIL", "T", "CSS", "BMS" ] # irrelevant if stationselection is 0
#MuonDetectorTool.ControlAlines     = GeneralControlAlines         # default is 111111

###############################################################################################


theApp.EvtMax = 10

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [
#    '/afs/usatlas.bnl.gov/users/dladams/data/calib0.005145.PythiaZmumu.simul.HITS.v12003101_tid003432._00001.pool.root.1'
'/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00001.pool.root.1',
]

#    '/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00006.pool.root.1',
#    '/afs/le.infn.it/user/s/spagnolo/projAtlas/hlt/stef/data/mc12.csc120.007218.digit.muminus_pt20GeV._00001.pool.root']

######################### here the tools

#from MuonEventAthenaPool.MuonEventAthenaPoolConf import *

#from MuonCablingServers.MuonCablingServersConfig import *
#from RPCcablingSim.RPCcablingSimConfig import *
#from RPCcabling.RPCcablingConfig import *

from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool()
CscRdoToCscPrepDataTool.OutputLevel = INFO
ToolSvc += CscRdoToCscPrepDataTool
print CscRdoToCscPrepDataTool


######################### here Alg. sequence

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()



from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
CscRdoToCscPrepData = CscRdoToCscPrepData()
#CscRdoToCscPrepData.DecodingTool = ToolSvc.CscPrepDataProviderTool
CscRdoToCscPrepData.PrintPrepData = True
from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
CscRdoToCscPrepData.RegSel_CSC = makeRegSelTool_CSC()

topSequence += CscRdoToCscPrepData
