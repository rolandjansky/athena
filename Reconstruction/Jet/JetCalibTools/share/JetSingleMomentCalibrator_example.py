
from JetRec.JetGetters import *  

# create a new ajet algorithm
Kt5LCalg = make_StandardJetGetter('Kt', 0.5, 'LCTopo').jetAlgorithmHandle()
Kt5LCalg.OutputLevel = 3

from JetCalibTools.JetCalibToolsConf import *
#instantiate a calibration JetAlgTool
jetcalibrationtool = JetCalibrationTool("CalibSingleMoment")
jetcalibrationtool.InplaceCorrection = True

# set its calibrator tool to JetSingleMomentCalibrator (here using the ara/athena wrapping)
jetcalibrationtool.Calibrator = JetCalibratorAraWrapper_JetCalibratorSingleMoment_()
## set some properties :
jetcalibrationtool.Calibrator.MomentName = 'WIDTH'

# add the calibration JetAlgTool to the tool sequence
Kt5LCalg.AlgTools += [jetcalibrationtool]
