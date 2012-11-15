###############################################################
#
# Job options file
#
###############################################################
#
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()

from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()

# default ?
DetDescrCnvSvc.IdDictFromRDB = True
# Default UseLArCoolPool is True
#UseLArCoolPool=True

# "LArIdMapFix>=7 <=> newFcal=true, means commisioning version, i.e. post 12.0.x release
LArIdMapFix=7

if LArIdMapFix>=7:
    DetDescrVersion="ATLAS-CSC-02-00-00"
#    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml"
#    DetDescrCnvSvc.CaloIDFileName = "IdDictCalorimeter_L1Onl.xml"
    include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
#    include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
else:    
    DetDescrVersion="ATLAS-CSC-01-00-00"
#    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05.xml"
#    DetDescrCnvSvc.CaloIDFileName = "IdDictCalorimeter_DC3-05.xml"
    include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
    
#setup GeoModel
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )
#GeoModelSvc = Service( "GeoModelSvc" )

include( "LArIdCnv/LArIdCnv_joboptions.py" )
# note : LArIdCnv includes LArTools_jobOptions.py, which includes CaloIdCnv, which includes CaloTriggerTool


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += ["TestLArHardwareID" ] 
theApp.TopAlg += ["TestLArHWID_Algo" ]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 2
MessageSvc.setDebug += [ "LArEM_ID" ];
MessageSvc.setDebug += [ "LArOnOffIdMap" ];
MessageSvc.infoLimit   = 200000
MessageSvc.debugLimit  = 200000
# 
# Set flag to check ids
# ----------------------
DetDescrCnvSvc.DoIdChecks = False;
theApp.EvtMax = 1

#--------------------------------------
# Set Property - to select sub-detector 
#--------------------------------------
TestLArHWID_Algo = Algorithm( "TestLArHWID_Algo" )
#
# Declare Mode chosen to access dictionary
# ----------------------------------------
#TestLArHWID_Algo.Helper = "DIRECT" ;


# -------------------------------------------------
#  Detector       |  ALL, EMB, EMEC, HEC, FCAL     |
#  SubDetector    |  ALL, S0, S1, S2, S3           |
#--------------------------------------------------
TestLArHWID_Algo.Detector    = "HEC"
TestLArHWID_Algo.SubDetector = "ALL"


# -------------------------------------------------
#  OnlineTest        |       ON , OFF, FEB         |
#  Calibration       |       ON , OFF, EXCL        |
#--------------------------------------------------
TestLArHWID_Algo.OnlineTest  = "OFF"
TestLArHWID_Algo.Calibration = "OFF"

# -------------------------------------------------
# OfflineTest        |       ON, OFF               |
# -------------------------------------------------
# Connected          |       ON, OFF, ALL          |
#   for Connected channels,OfflineTest must be ON  |
# --------------------------------------------------
TestLArHWID_Algo.OfflineTest = "ON"
TestLArHWID_Algo.Connected   = "ON"

# -------------------------------------------------
# HighVoltage             |       ON, OFF          |
# -------------------------------------------------
# - HVlineToOffline       |       ON, OFF          |
# - HVelectrodeToOffline  |       ON, OFF          |
# -------------------------------------------------
TestLArHWID_Algo.HighVoltage          ="OFF"
TestLArHWID_Algo.HVlineToOffline      ="OFF"
TestLArHWID_Algo.HVelectrodeToOffline ="OFF"


#
#-------------------------------------------
# choose LAr and Calo dictionnary.
#-------------------------------------------
#DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05.xml"
#DetDescrCnvSvc.CaloIDFileName   = "IdDictCalorimeter_DC3-05.xml" ;

# Tool for EMECfix in LArTools
# ToolSvc = Service("ToolSvc")
# ToolSvc.LArCablingService.LArCablingEMECfix = "OFF"

#



#
# End of job options file
#
###############################################################
