#==============================================================
# Job options file for the ALFA_LocRec package
#==============================================================

from ALFA_LocRecCorr.ALFA_LocRecCorrConf import ALFA_LocRecCorr

alfaLocRecCorr =ALFA_LocRecCorr("ALFA_LocRecCorr")

# select between a real data (1) or a simulation mode (0), # specify a detector metrology type and its source
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
	alfaLocRecCorr.DataType        = 1
else:
	alfaLocRecCorr.DataType        = 0
alfaLocRecCorr.MetrologyType   = 3
alfaLocRecCorr.MetrologySource = ""

#if alfaLocRecCorr.MetrologyType==3:
#	from IOVDbSvc.CondDB import conddb
#	conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_FWD;dbname=COMP200</db> /FWD/ALFA/position_calibration <tag>FWDALFAposition_calibration-run373-v2</tag>")

# specify position type and fiber metrology properties of the RP
alfaLocRecCorr.B7L1U_PosType        = 1
alfaLocRecCorr.B7L1U_MDGeometryType = 2
alfaLocRecCorr.B7L1U_ODGeometryType = 2
alfaLocRecCorr.B7L1U_MDSource       = ""
alfaLocRecCorr.B7L1U_ODSource       = ""

alfaLocRecCorr.B7L1L_PosType        = 1
alfaLocRecCorr.B7L1L_MDGeometryType = 2
alfaLocRecCorr.B7L1L_ODGeometryType = 2
alfaLocRecCorr.B7L1L_MDSource       = ""
alfaLocRecCorr.B7L1L_ODSource       = ""

alfaLocRecCorr.A7L1U_PosType        = 1
alfaLocRecCorr.A7L1U_MDGeometryType = 2
alfaLocRecCorr.A7L1U_ODGeometryType = 2
alfaLocRecCorr.A7L1U_MDSource       = ""
alfaLocRecCorr.A7L1U_ODSource       = ""

alfaLocRecCorr.A7L1L_PosType        = 1
alfaLocRecCorr.A7L1L_MDGeometryType = 2
alfaLocRecCorr.A7L1L_ODGeometryType = 2
alfaLocRecCorr.A7L1L_MDSource       = ""
alfaLocRecCorr.A7L1L_ODSource       = ""

alfaLocRecCorr.A7R1U_PosType        = 1
alfaLocRecCorr.A7R1U_MDGeometryType = 2
alfaLocRecCorr.A7R1U_ODGeometryType = 2
alfaLocRecCorr.A7R1U_MDSource       = ""
alfaLocRecCorr.A7R1U_ODSource       = ""

alfaLocRecCorr.A7R1L_PosType        = 1
alfaLocRecCorr.A7R1L_MDGeometryType = 2
alfaLocRecCorr.A7R1L_ODGeometryType = 2
alfaLocRecCorr.A7R1L_MDSource       = ""
alfaLocRecCorr.A7R1L_ODSource       = ""

alfaLocRecCorr.B7R1U_PosType        = 1
alfaLocRecCorr.B7R1U_MDGeometryType = 2
alfaLocRecCorr.B7R1U_ODGeometryType = 2
alfaLocRecCorr.B7R1U_MDSource       = ""
alfaLocRecCorr.B7R1U_ODSource       = ""

alfaLocRecCorr.B7R1L_PosType        = 1
alfaLocRecCorr.B7R1L_MDGeometryType = 2
alfaLocRecCorr.B7R1L_ODGeometryType = 2
alfaLocRecCorr.B7R1L_MDSource       = ""
alfaLocRecCorr.B7R1L_ODSource       = ""


#Transformations - if MetrologyType ==1 one can use these user transformations, to activate them, set up transforms to TRUE
if alfaLocRecCorr.MetrologyType==1:
	alfaLocRecCorr.bIsTransformInDetector        = [False, False, False, False, False, False, False, False]
	alfaLocRecCorr.bIsTransformInStation         = [False, False, False, False, False, False, False, False]

	alfaLocRecCorr.pointTransformInDetectorB7L1U = [-77.5, -170.2, 114.0]
	alfaLocRecCorr.pointTransformInDetectorB7L1L = [-77.5, -170.2, 114.0]
	alfaLocRecCorr.pointTransformInDetectorA7L1U = [-77.5, -170.2, 114.0]
	alfaLocRecCorr.pointTransformInDetectorA7L1L = [-77.5, -170.2, 114.0]
	alfaLocRecCorr.pointTransformInDetectorA7R1U = [-77.5, -170.2, 114.0]
	alfaLocRecCorr.pointTransformInDetectorA7R1L = [-77.5, -170.2, 114.0]
	alfaLocRecCorr.pointTransformInDetectorB7R1U = [-77.5, -170.2, 114.0]
	alfaLocRecCorr.pointTransformInDetectorB7R1L = [-77.5, -170.2, 114.0]

	alfaLocRecCorr.vecTransformInDetectorB7L1U   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInDetectorB7L1L   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInDetectorA7L1U   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInDetectorA7L1L   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInDetectorA7R1U   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInDetectorA7R1L   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInDetectorB7R1U   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInDetectorB7R1L   = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

	alfaLocRecCorr.vecTransformInStationB7L1U    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInStationB7L1L    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInStationA7L1U    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInStationA7L1L    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInStationA7R1U    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInStationA7R1L    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInStationB7R1U    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	alfaLocRecCorr.vecTransformInStationB7R1L    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]



topSequence += alfaLocRecCorr
