import os

# database information is set by including "MuonCalibDbOperations/CalibDbInfo.py" in your job options file

appendToExistingFile_flag=False
if "appendToExistingFile" in dir():
	appendToExistingFile_flag = appendToExistingFile
	
if os.path.exists(coolFileName) and not appendToExistingFile_flag:
	print "FATAL File", coolFileName, "already exists"
	print "FATAL remove file or set 'appendToExistingFile = True'"
	sys.exit(1)

### Setup Athena common flags
#include ("AthenaCommon/AthenaCommonFlags.py")
#include.block("AthenaCommon/AthenaCommonFlags.py")

#include( "IOVDbSvc/IOVRecExCommon.py" )

doMuonCalibAtlas=True


from AthenaCommon.AppMgr import ServiceMgr, ToolSvc
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
ToolSvc = Service( "ToolSvc" )
globalflags.DataSource.set_Value_and_Lock('data')
#globalflags.Print()
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()

from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
if  "atlasVersion" in dir():
	GeoModelSvc.AtlasVersion = atlasVersion


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

from MuonCalibDbOperations.CalibrationSourceConfig import *
from MuonCalibDbOperations.CalibrationTargetFolderConfig import *
from MuonCalibDbOperations.genGasmonSource import *
from MuonCalibDbOperations.CalibrationDefaultSourceConfig import *
import cx_Oracle


if autoFillGasmon:
	reader_connection=cx_Oracle.connect(dbr[calibdb], dbr_password[calibdb],tns[calibdb])
	reader_cursor=reader_connection.cursor()
	gasmon_source=MuonCalib__genGasmonSource(toroidOn, calibrationTime, reader_cursor)
	if "GasmonDriftTimeOffsetsVsR" in dir():
		gasmon_source.DriftTimeOffsetsVsR=GasmonDriftTimeOffsetsVsR
	gasmon_source.BFieldCorrectionApplied = not toroidOn
	gasmon_source.TimeSlewingApplied = False
	

for source_config in MuonCalib__gCalibrationSourceConfigs:
	MuonCalibDbCalibrationSource = source_config.GenCalibrationSource(readerPassword)
	ToolSvc += MuonCalibDbCalibrationSource
	CoolInserter.CalibrationSources.append(MuonCalibDbCalibrationSource)
	print MuonCalibDbCalibrationSource

MuonCalib_gCalibrationDefaultSourceConfig.ConfigureTool(ToolSvc, CoolInserter)

dbname="CONDBR2"
if "isMC" in dir():
	if isMC:
		dbname="OFLP200"

CoolInserter.CoolConnectionString = "sqlite://;schema=" + coolFileName + ";dbname="+dbname

for target_config in MuonCalib__gCalibrationTargetConfigs:
	if globalConditionsTag:
		target_config.ResolveGlobalTag(globalConditionsTag)
	target_config.AddToCoolInserter(CoolInserter)
if not regionToInsert:
	regionToInsert=""

RegionSelectionSvc.Region = regionToInsert

CoolInserter.IOVStart=targetIov[0]
CoolInserter.IOVEnd=targetIov[1]

print CoolInserter
