#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_InclBrl_4 import SLHC_Setup
SLHC_Setup = SLHC_Setup()

if rec.OutputFileNameForRecoStep() == 'RAWtoESD' or rec.OutputFileNameForRecoStep() == 'ESDtoDPD':
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False  
