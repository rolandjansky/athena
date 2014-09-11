# Call as UserAlgs

from JetRec.JetRecConf import *
from JetRecTools.JetRecToolsConf import *

# reconstruct Kt5LCTopoJets
from JetRec.JetGetters import *
Kt5LCalg = make_StandardJetGetter('Kt', 0.5, 'LCTopo').jetAlgorithmHandle()

from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
jetcalibrationtool2 = JetCalibrationTool("CalibDummy")

from JetCalibTools.JetCalibToolsConf import *
jetcalibrationtool2.Calibrator = MCNumInversionResponse()
jetcalibrationtool2.Calibrator.CalibrationTag = "NumInv"

# Enter Eta Bins... This is the correct format (don't start with zero, and don't insert 4.5 at the end)
jetcalibrationtool2.Calibrator.EtaBins = [0.3,0.8,1.4,1.5,1.6,1.7,2.1,2.8,2.9,3.0,3.1,3.2,3.3,3.4,3.5,3.6]

# Enter Calib Constants... Just Cut and Paste from the website and format
# Those are the constants for Cone4TowerJets
jetcalibrationtool2.Calibrator.CalibConstants ="[[-0.574, -8.71 , 35.2  ,-39.6][-0.465, -9.59 , 37.2  ,-41.2][-0.238, -12.55, 45.6  ,-48.8][-1.209, -5.23 , 26.1  ,-30.4][-0.055, -12.21, 41.3  ,-42.0][-0.155, -10.7 , 37.1  ,-38.9][0.418 , -14.4 , 47.5  ,-49.6][-0.219, -7.94 , 28.4  ,-30.8][-0.980, -1.20 , 7.27  ,-7.91][-1.805, 5.22  , -11.40, 10.74][-2.518, 10.92 , -29.1 , 29.2][-2.661, 7.80  , -14.0 , 11.1][-4.10 , 16.5  , -34.2 , 29.2][-2.969, 6.99  , -7.12 , 3.68][-1.701, 5.46  , -20.2 , 28.0][-3.117, 25.16 , -93.1 , 112.3][-1.697, 8.03  , -29.8 , 39.8 ]]"

# Use Jet Moment corrections for now
#jetcalibrationtool2.InplaceCorrection = True
# Store both moments and calibration
#jetcalibrationtool2.WriteMoments = True

# Append the jet tool
Kt5LCalg.AlgTools += [jetcalibrationtool2]


