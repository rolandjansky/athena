
#----
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



include ("AmdcAth/AmdcAth_jobOptions.py")

###################### try here the DB stuff
from IOVDbSvc.CondDB import conddb
conddb.addFolder("MUONALIGN","/MUONALIGN/MDT/BARREL <tag>HEAD</tag>",className='CondAttrListCollection')

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
condSequence+=MuonAlignmentCondAlg('MuonAlignmentCondAlg')
MuonAlignmentCondAlg.ParlineFolders = ["/MUONALIGN/MDT/BARREL"]

from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.UseConditionDb = 1
MuonDetectorTool.OutputLevel = VERBOSE
#MuonDetectorTool.CachingFlag       = 1 # default is 1 
MuonDetectorTool.FillCacheInitTime = 1 # default is 0
#MuonDetectorTool.StationSelection  = 0 # default is 0
#MuonDetectorTool.SelectedStations  = [ "EIL", "T", "CSS", "BMS" ] # irrelevant if stationselection is 0
#MuonDetectorTool.ControlAlines     = GeneralControlAlines         # default is 111111

###############################################################################################

###################################################################
from MuonGeoModelTest.MuonGeoModelTestConf import MuonGMTestOnPrd
MuonGMTestOnPrd = MuonGMTestOnPrd()
MuonGMTestOnPrd.OutputLevel = DEBUG
###################################################################

######################


theApp.EvtMax = 10

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ['/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00001.pool.root.1', '/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00006.pool.root.1','/afs/le.infn.it/user/s/spagnolo/projAtlas/hlt/stef/data/mc12.csc120.007218.digit.muminus_pt20GeV._00001.pool.root']
#ServiceMgr.EventSelector.InputCollections = ['/afs/le.infn.it/user/s/spagnolo/projAtlas/hlt/stef/data/mc12.csc120.007218.digit.muminus_pt20GeV._00001.pool.root']



from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

include ("MuonRdoToPrepData/MuonRdoToMuonPrepData_jobOptions.py")
#from MuonRdoToPrepData.MuonRdoToPrepDataGenConf import MdtRdoToMdtPrepData
#MdtRdoToMdtPrepData = MdtRdoToMdtPrepData()
MdtRdoToMdtPrepData.CalibratePrepData = False
from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT
MdtRdoToMdtPrepData.RegSel_MDT = makeRegSelTool_MDT()

topSequence += MuonGMTestOnPrd
