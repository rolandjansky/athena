setGlobalTag='OFLCOND-RUN12-SDR-31'
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/PIXEL/PixelModuleFeMask','PixelModuleFeMask-SIM-MC16-000-03')
conddb.addOverride("/TRT/Calib/PID_NN", "TRTCalibPID_NN_v1")

