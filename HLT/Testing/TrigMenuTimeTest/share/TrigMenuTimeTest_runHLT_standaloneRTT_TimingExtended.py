# This is just a wrapper job option made for RTT running

from AthenaCommon.Include import include

include("TriggerRelease/runHLT_standalone.py")

if TriggerFlags.doLVL2():
    #topSequence.TrigSteer_L2.MonTools['L2SteeringTime'].NumberOfHistBins = 200
    topSequence.TrigSteer_L2.MonTools['L2SteeringTime'].TimerHistLimits  = [0,2000]
    #topSequence.TrigSteer_L2.MonTools['L2SteeringTime'].GroupedTimers={
    #    "TrigSteer_L2_Chains": "TrigSteer_L2_Chain_.+",
    #    "TrigSteer_L2_Sequences": "TrigSteer_L2_sequence_.+",
    #    }
if TriggerFlags.doEF():
    #topSequence.TrigSteer_EF.MonTools['EFSteeringTime'].NumberOfHistBins = 200
    topSequence.TrigSteer_EF.MonTools['EFSteeringTime'].TimerHistLimits = [0,2000]
    #topSequence.TrigSteer_EF.MonTools['EFSteeringTime'].GroupedTimers={
    #    "TrigSteer_EF_Chains": "TrigSteer_EF_Chain_.+",
    #    "TrigSteer_EF_Sequences": "TrigSteer_EF_sequence_.+",
    #    }
