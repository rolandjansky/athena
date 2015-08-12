#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

# https://its.cern.ch/jira/browse/ATLASRECTS-2321
if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False 
