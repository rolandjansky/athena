# --- Full job is a list of algorithms ---
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# --- Add top algorithms to be run ---
from MuonCalibrationFit.MuonCalibrationFitConf import MuonCalibrationFit
job += MuonCalibrationFit( 'MuonCalibrationFit' )

# --- Set output level threshold ( DEBUG, INFO, WARNING, ERROR, FATAL ) ---
job.MuonCalibrationFit.OutputLevel = DEBUG

# --- Set application parameters --- 
theApp.EvtMax = 1
