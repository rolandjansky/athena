### Setup Athena common flags
include ("AthenaCommon/AthenaCommonFlags.py")
include.block("AthenaCommon/AthenaCommonFlags.py")

include( "IOVDbSvc/IOVRecExCommon.py" )

doMuonCalibAtlas=True


from AthenaCommon.AppMgr import ServiceMgr, ToolSvc
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
ToolSvc = Service( "ToolSvc" )
GlobalFlags.DataSource.set_data()
GlobalFlags.Print()
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()

from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.AtlasVersion = "ATLAS-GEO-08-00-00"


from MuonCalibDbOperations.MuonCalibDbOperationsConf import MuonCalib__CoolInserter, MuonCalib__MuonCalibDbCalibrationSource, MuonCalib__MuonCalibDefaultCalibrationSource
CoolInserter=MuonCalib__CoolInserter()
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += CoolInserter


#cheat athena event loop 
theApp.EvtMax = 1

from MuonCalibStandAloneBase.MuonCalibStandAloneBaseConf import RegionSelectionSvc
RegionSelectionSvc = RegionSelectionSvc()
RegionSelectionSvc.PrintList = True
ServiceMgr += RegionSelectionSvc





################################################################################
################################################################################
## START OF USER SETTINGS                                                     ##
################################################################################
################################################################################


################################################################################
#select calibration sources
################################################################################

CalibrationDatabase=True
DefaultValues=False

################################################################################
#parameters of calibration database connection
################################################################################

if CalibrationDatabase:
	MuonCalibDbCalibrationSource = MuonCalib__MuonCalibDbCalibrationSource()
	ToolSvc += MuonCalibDbCalibrationSource	
	CoolInserter.CalibrationSources.append( MuonCalibDbCalibrationSource )

	MuonCalibDbCalibrationSource.ConnectionString =  "oracle://INTR/ATLAS_MUONCALIB_READER"
#select ROME/MUNICH/MICHIGAN
	MuonCalibDbCalibrationSource.WorkingSchema = "ATLAS_MUONCALIB_MPI_META"
#	MuonCalibDbCalibrationSource.WorkingSchema = "ATLAS_MUONCALIB_RM_META"
#	MuonCalibDbCalibrationSource.WorkingSchema = "ATLAS_MUONCALIB_UM_META"
	MuonCalibDbCalibrationSource.UserName = "ATLAS_MUONCALIB_READER"
	MuonCalibDbCalibrationSource.Password = "XXXXXXX"
	MuonCalibDbCalibrationSource.SiteName = "MP"
	MuonCalibDbCalibrationSource.HeadId = 109
	MuonCalibDbCalibrationSource.Region = "[B??]"
	
	MuonCalibDbCalibrationSource.StoreRT=True
	MuonCalibDbCalibrationSource.StoreT0=True
	
	
	print MuonCalibDbCalibrationSource

################################################################################
# parameters for default values
################################################################################

if DefaultValues:
	MuonCalibDefaultCalibrationSource = MuonCalib__MuonCalibDefaultCalibrationSource()
	ToolSvc += MuonCalibDefaultCalibrationSource	
	CoolInserter.CalibrationSources.append( MuonCalibDefaultCalibrationSource ) 
	
#endcap chambes
	MuonCalibDefaultCalibrationSource.T0Regions += [ "[E??]" ]
	MuonCalibDefaultCalibrationSource.T0 += [ 500 ]
#barrel chambers
	MuonCalibDefaultCalibrationSource.T0Regions += [ "[B??]" ]
	MuonCalibDefaultCalibrationSource.T0 += [ 600 ]
#rt regon
	MuonCalibDefaultCalibrationSource.RtRegions += [ "" ]
	MuonCalibDefaultCalibrationSource.RtFiles += [ "Rt_BML_2_2.dat" ]		
	print MuonCalibDefaultCalibrationSource


################################################################################
#parameters for cool database
################################################################################

CoolInserter.CoolConnectionString = "sqlite://;schema=test.db;dbname=COMP200"
#online folders

CoolInserter.T0Folder += [ "/MDT/T0" ]
CoolInserter.T0Tag += [ "MDTT0_RPCwBeam-02" ]
CoolInserter.T0FolderCompress += [True]
CoolInserter.RtFolder += [ "/MDT/RT" ]
CoolInserter.RtTag += [ "MDTRT_RPCwBeam-02" ]
CoolInserter.RtFolderCompress += [True]


#if set to -1 the IOV from the header is used - IOV is inclusive
CoolInserter.IOVStart=-1
CoolInserter.IOVEnd=-1


################################################################################
#replication options
################################################################################

#select region to replicate
RegionSelectionSvc.Region = ""

print CoolInserter
