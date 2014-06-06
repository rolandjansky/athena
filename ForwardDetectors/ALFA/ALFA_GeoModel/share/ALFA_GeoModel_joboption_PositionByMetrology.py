# Example of setup which position of Roman Pots by metrology with real fiber metrology
from IOVDbSvc.CondDB import conddb
from ALFA_GeoModel.ALFA_GeoModelConf import ALFA_DetectorTool

def getALFA_DetectorTool(name="ALFA_DetectorTool"):
	#conddb.addFolder("FWD_OFL","/FWD/ALFA/position_calibration")
	conddb.addFolderSplitOnline('FWD','/FWD/Onl/ALFA/position_calibration','/FWD/ALFA/position_calibration')

	#instatiate the tool
	theALFA_DetectorTool=ALFA_DetectorTool(name)
	theALFA_DetectorTool.MetrologyType=2

	# setup LVDT for each pot - applicable only when MetrologyType=2
	theALFA_DetectorTool.B7L1U_CurrentLVDT = 7
	theALFA_DetectorTool.B7L1L_CurrentLVDT = -7
	theALFA_DetectorTool.A7L1U_CurrentLVDT = 7
	theALFA_DetectorTool.A7L1L_CurrentLVDT = -7
	theALFA_DetectorTool.A7R1U_CurrentLVDT = 7
	theALFA_DetectorTool.A7R1L_CurrentLVDT = -7
	theALFA_DetectorTool.B7R1U_CurrentLVDT = 7
	theALFA_DetectorTool.B7R1L_CurrentLVDT = -7

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

	return theALFA_DetectorTool
