#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

if rec.OutputFileNameForRecoStep() == 'RAWtoESD' or rec.OutputFileNameForRecoStep() == 'ESDtoDPD':
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False  

# If /Indet/IBLDist is available, InnerDetector/InDetDetDescr/InDetReadoutGeometry/src/InDetDetectorManager.cxx will try to perform alignment and lead to crash
# InnerDetector/InDetConditions/InDetCondFolders/python/InDetAlignFolders.py configures /Indet/IBLDist
from IOVDbSvc.CondDB import conddb
conddb.blockFolder("/Indet/IBLDist")

from AthenaCommon.DetFlags import DetFlags
if hasattr(ServiceMgr, 'PixelLorentzAngleSvc'):
    # this avoid to use hard-coded valus for IBL in the LorentzAngleSvc
    print "--> Running with ITkLayout" 
    ServiceMgr.PixelLorentzAngleSvc.ITkLayout = True
    
