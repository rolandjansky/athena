#######################################################
#include ( "../run/services.py" )

from JetRec.JetRecConf import *
from JetRecTools.JetRecToolsConf import *
from JetCalibTools.JetCalibToolsConf import *

TrackCorrTool = JetCalibrationTool("TrackCorrTool")
TrackCorrTool.Calibrator = TrackBasedResponseCorrection()
TrackCorrTool.Calibrator.CalibrationTag = "TrackCorr"
TrackCorrTool.Calibrator.WriteMoments = False
TrackCorrTool.Calibrator.OutputLevel = DEBUG

# Enter Eta Bins... This is the correct format (don't start with zero, and don't insert 4.5 at the end)
TrackCorrTool.Calibrator.EtaBins = [0.8,1.4]

# These are the constants for AntiKt4TopoJets
TrackCorrTool.Calibrator.nTrackFit ="[[1.05754,1.10115,0.0164969,-0.0046288,-0.030435,0.00912633,4.48061e-05,0.00117745,0.0118138,-1.12836,0.0190392],[1.02877,1.11953,0.00851397,0.000589692,-0.0283504,0.00451346,-0.000356189,0.00111315,0.00361306,-1.10473,0.00975281]]"

# Invoke the Jet Getters by overriding the calibration tool used
from JetRec.JetGetters import *
AntiKt4H1alg = make_StandardJetGetter('AntiKt', 0.4, 'Topo', calibTools=[TrackCorrTool]).jetAlgorithmHandle()
AntiKt4H1alg.OutputLevel = 3 
