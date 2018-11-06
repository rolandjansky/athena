###############################################################
#
# Job options file
#
###############################################################
#
include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.JobProperties import jobproperties

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')

from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()

# default ?
# Default UseLArCoolPool is True
#UseLArCoolPool=True

# "LArIdMapFix>=7 <=> newFcal=true, means commisioning version, i.e. post 12.0.x release
LArIdMapFix=7

if LArIdMapFix>=7:
    jobproperties.Global.DetDescrVersion="ATLAS-R2-2015-03-01-00"
#    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml"
#    DetDescrCnvSvc.CaloIDFileName = "IdDictCalorimeter_L1Onl.xml"
    include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
#    include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
else:    
    jobproperties.Global.DetDescrVersion="ATLAS-CSC-01-00-00"
#    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05.xml"
#    DetDescrCnvSvc.CaloIDFileName = "IdDictCalorimeter_DC3-05.xml"
    include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
    
#setup GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
#GeoModelSvc = Service( "GeoModelSvc" )

include( "LArIdCnv/LArIdCnv_joboptions.py" )
# note : LArIdCnv includes LArTools_jobOptions.py, which includes CaloIdCnv, which includes CaloTriggerTool

svcMgr.DetDescrCnvSvc.IdDictFromRDB = True
svcMgr.EventSelector.RunNumber= 10000000
svcMgr.IOVDbSvc.GlobalTag='OFLCOND-MC16-SDR-24'

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
from LArRecUtils.LArRecUtilsConf import LArHVIdMappingAlg
condSequence+=LArHVIdMappingAlg(ReadKey="/LAR/Identifier/HVLineToElectrodeMap")
from LArConditionsCommon import LArHVDB

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from TestLArHardwareID.TestLArHardwareIDConf import TestLArHWID_Algo 
testLArHWID_Algo = TestLArHWID_Algo()
topSequence += testLArHWID_Algo

#from LArCabling.LArCablingConf import LArHVCablingTool
#hvcab=LArHVCablingTool()
#hvcab.MappingFolder="/LAR/Identifier/HVLineToElectrodeMap"
#svcMgr.ToolSvc += hvcab
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING
#MessageSvc.setDebug += [ "LArEM_ID" ];
#MessageSvc.setDebug += [ "LArOnOffIdMap" ];
MessageSvc.infoLimit   = 200000
MessageSvc.debugLimit  = 200000
# 
# Set flag to check ids
# ----------------------
svcMgr.DetDescrCnvSvc.DoIdChecks = False;
theApp.EvtMax = 1

#--------------------------------------
# Set Property - to select sub-detector 
#--------------------------------------
#
# Declare Mode chosen to access dictionary
# ----------------------------------------
#TestLArHWID_Algo.Helper = "DIRECT" ;


# -------------------------------------------------
#  Detector       |  ALL, EMB, EMEC, HEC, FCAL     |
#  SubDetector    |  ALL, S0, S1, S2, S3           |
#--------------------------------------------------
testLArHWID_Algo.Detector    = "ALL"
testLArHWID_Algo.SubDetector = "ALL"


# -------------------------------------------------
#  OnlineTest        |       ON , OFF, FEB         |
#  Calibration       |       ON , OFF, EXCL        |
#--------------------------------------------------
testLArHWID_Algo.OnlineTest  = "OFF"
testLArHWID_Algo.Calibration = "OFF"

# -------------------------------------------------
# OfflineTest        |       ON, OFF               |
# -------------------------------------------------
# Connected          |       ON, OFF, ALL          |
#   for Connected channels,OfflineTest must be ON  |
# --------------------------------------------------
testLArHWID_Algo.OfflineTest = "OFF"
testLArHWID_Algo.Connected   = "OFF"

# -------------------------------------------------
# HighVoltage             |       ON, OFF          |
# -------------------------------------------------
# - HVlineToOffline       |       ON, OFF          |
# - HVelectrodeToOffline  |       ON, OFF          |
# -------------------------------------------------
testLArHWID_Algo.HighVoltage          ="ON"
testLArHWID_Algo.HVlineToOffline      ="ON"
testLArHWID_Algo.HVelectrodeToOffline ="ON"

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
