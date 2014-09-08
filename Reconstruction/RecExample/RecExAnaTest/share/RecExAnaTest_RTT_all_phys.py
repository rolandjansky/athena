include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

include ("RecExCond/RecExCommon_flags.py")
TriggerFlags.triggerMenuSetup = 'Physics_lumi1E31_simpleL1Calib'
#TriggerFlags.L1PrescaleSet = ''
#TriggerFlags.HLTPrescaleSet = ''
TriggerFlags.useL1CaloCalibration = False 
TriggerFlags.L1PrescaleSet  = 'L1Prescales100_Physics_lumi1E31_simpleL1Calib'
TriggerFlags.HLTPrescaleSet = 'HLTPrescales100_Physics_lumi1E31_simpleL1Calib'
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")

