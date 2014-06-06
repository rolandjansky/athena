# Example of setup which nominal position of Roman Pots together with User Corrections, real fiber metrology
# Here we apply user corrections to the Roman Pot A7L1U in following manner:
#	1. rotation of inner detector parts in RP body by 9 mrad along z-axis in the TrackPoint [-77.5, -170.2, 114.0] mm
#	2. shift of inner detector parts by (0.1, -0.3, 0.2) mm
#	3. shift of whole Roman Pot in station by (1.0, -2.0, 5.0) mm
from IOVDbSvc.CondDB import conddb
from ALFA_GeoModel.ALFA_GeoModelConf import ALFA_DetectorTool

def getALFA_DetectorTool(name="ALFA_DetectorTool"):
	#conddb.addFolder("FWD_OFL","/FWD/ALFA/position_calibration")
	conddb.addFolderSplitOnline('FWD','/FWD/Onl/ALFA/position_calibration','/FWD/ALFA/position_calibration')

	#instatiate the tool
	theALFA_DetectorTool=ALFA_DetectorTool(name)
	theALFA_DetectorTool.MetrologyType=1

	#set 1 for nominal fiber positions or 2 for position by metrology
	theALFA_DetectorTool.B7L1U_MDGeometryType = 2
	theALFA_DetectorTool.B7L1U_ODGeometryType = 2
	theALFA_DetectorTool.B7L1L_MDGeometryType = 2
	theALFA_DetectorTool.B7L1L_ODGeometryType = 2
	theALFA_DetectorTool.A7L1U_MDGeometryType = 2
	theALFA_DetectorTool.A7L1U_ODGeometryType = 2
	theALFA_DetectorTool.A7L1L_MDGeometryType = 2
	theALFA_DetectorTool.A7L1L_ODGeometryType = 2
	theALFA_DetectorTool.A7R1U_MDGeometryType = 2
	theALFA_DetectorTool.A7R1U_ODGeometryType = 2
	theALFA_DetectorTool.A7R1L_MDGeometryType = 2
	theALFA_DetectorTool.A7R1L_ODGeometryType = 2
	theALFA_DetectorTool.B7R1U_MDGeometryType = 2
	theALFA_DetectorTool.B7R1U_ODGeometryType = 2
	theALFA_DetectorTool.B7R1L_MDGeometryType = 2
	theALFA_DetectorTool.B7R1L_ODGeometryType = 2

	# --------- Transformations -----------------------------------------------------------------------

	# transformations in the detector and/or in the station
	# if yes True, else False                  [B7L1U, B7L1L, A7L1U, A7L1L, A7R1U, A7R1L, B7R1U, B7R1L]
	theALFA_DetectorTool.bIsTransformInDetector = [False, False, True, False, False, False, False, False]
	theALFA_DetectorTool.bIsTransformInStation  = [False, False, True, False, False, False, False, False]

	# rotation point for the transformations in the detector
	# point(x,y,z)[mm] - with respect to the RPPin1
	theALFA_DetectorTool.pointTransformInDetectorB7L1U = [0.0, 0.0, 0.0]
	theALFA_DetectorTool.pointTransformInDetectorB7L1L = [0.0, 0.0, 0.0]
	theALFA_DetectorTool.pointTransformInDetectorA7L1U = [-77.5, -170.2, 114.0]
	theALFA_DetectorTool.pointTransformInDetectorA7L1L = [0.0, 0.0, 0.0]
	theALFA_DetectorTool.pointTransformInDetectorA7R1U = [0.0, 0.0, 0.0]
	theALFA_DetectorTool.pointTransformInDetectorA7R1L = [0.0, 0.0, 0.0]
	theALFA_DetectorTool.pointTransformInDetectorB7R1U = [0.0, 0.0, 0.0]
	theALFA_DetectorTool.pointTransformInDetectorB7R1L = [0.0, 0.0, 0.0]

	# transformation parametres for the transformations in the detector
	# angle[rad], vecRotation(x,y,z), vecTranslation(x,y,z)[mm]
	theALFA_DetectorTool.vecTransformInDetectorB7L1U = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInDetectorB7L1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInDetectorA7L1U = [0.009, 0.0, 0.0, 1.0, 0.1, -0.3, 0.2]
	theALFA_DetectorTool.vecTransformInDetectorA7L1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInDetectorA7R1U = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInDetectorA7R1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInDetectorB7R1U = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInDetectorB7R1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

	# transformation parametres for the transformations in the station
	# angle[rad], vecRotation(x,y,z), vecTranslation(x,y,z)[mm]
	theALFA_DetectorTool.vecTransformInStationB7L1U = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInStationB7L1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInStationA7L1U = [0.0, 0.0, 0.0, 0.0, 1.0, -2.0, 5.0]
	theALFA_DetectorTool.vecTransformInStationA7L1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInStationA7R1U = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInStationA7R1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInStationB7R1U = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	theALFA_DetectorTool.vecTransformInStationB7R1L = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

	return theALFA_DetectorTool
