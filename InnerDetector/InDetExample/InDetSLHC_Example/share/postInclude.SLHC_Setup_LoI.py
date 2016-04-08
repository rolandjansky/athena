#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup_LoI import SLHC_Setup
SLHC_Setup = SLHC_Setup()

if rec.OutputFileNameForRecoStep() == 'RAWtoESD' or rec.OutputFileNameForRecoStep() == 'ESDtoDPD':
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False  
