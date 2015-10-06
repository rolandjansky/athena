#########################################################
#                                                  v14  #
#                                                       #
# TRT_Digitization/postInclude.OverrideTRTparameters.py #
#                                                       #
# Overrride default settings in TRTDigSettings.cxx      #
#                                                       #
# This is not a full list, just the parameters likely   #
# to be used in basic tuning...                         #
#                                                       #
# For a full list of overrridable parameters            #
# and a list of setttings used in the job:              #
#                                                       #
#   grep TRTDigSettings <job output>                    #
#                                                       #
#########################################################

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from Digitization.DigitizationFlags import digitizationFlags
trt = None
if not digitizationFlags.doXingByXingPileUp() and hasattr(job, 'TRTDigitization'):
    #Back-compatibility for 17.X.Y.Z releases
    if hasattr(job.TRTDigitization, 'DigitizationTool'):
        trt = job.TRTDigitization.DigitizationTool
    else:
        trt = job.TRTDigitization
else:
     from AthenaCommon.CfgGetter import getPublicTool
     trt = getPublicTool("TRTDigitizationTool")
if None == trt:
   raise AttributeError("TRTDigitization(Tool) not found.")

# Units are CLHEP: MeV, ns, mm

# Xenon threshold tunes
# 0.000285, 0.000300, 0.006710, 0.006450, 0.006720, 0.006830   00-10-73
# 0.000260, 0.000275, 0.006039, 0.005805, 0.006048, 0.006147   00-10-74-14 (with attenuation)
# 0.000260, 0.000275, 0.006047, 0.005477, 0.005916, 0.005680   00-11-07    (fine-tune of HT)
# 0.000260, 0.000275, 0.006576, 0.006016, 0.006390, 0.006074   01-00-24    (HT middle-bit fraction tune - wider shaping function)
# 0.000260, 0.000275, 0.005412, 0.004949, 0.005251, 0.005072   01-01-03    (Delta-ray suppression tune)
trt.Override_lowThresholdBar        = 0.000260
trt.Override_lowThresholdEC         = 0.000275
trt.Override_highThresholdBarShort  = 0.005412
trt.Override_highThresholdBarLong   = 0.004949
trt.Override_highThresholdECAwheels = 0.005251
trt.Override_highThresholdECBwheels = 0.005072
# TR efficiency for Xenon, defaults = 0.95, 1.00, 1.00
# 0.774, 0.932, 0.830 (Delta-ray suppression tune)
trt.Override_trEfficiencyBarrel = 0.774
trt.Override_trEfficiencyEndCapA = 0.932
trt.Override_trEfficiencyEndCapB = 0.830

# Argon thresholds (July 2014 tuning from Artem) TRT_Digitization-01-00-09
# 0.000070 0.000070 0.002446 0.002141 0.002235 0.002146   01-00-09
# 0.000070 0.000070 0.002660 0.002352 0.002414 0.002295   01-00-24    (HT middle-bit fraction tune - scaled from Xenon study)
trt.Override_lowThresholdBarArgon        = 0.000070
trt.Override_lowThresholdECArgon         = 0.000070
trt.Override_highThresholdBarShortArgon  = 0.002660
trt.Override_highThresholdBarLongArgon   = 0.002352
trt.Override_highThresholdECAwheelsArgon = 0.002414
trt.Override_highThresholdECBwheelsArgon = 0.002295
# TR efficiency for Argon, defaults = 0.55, 0.80
trt.Override_trEfficiencyBarrelArgon = 0.55
trt.Override_trEfficiencyEndCapAArgon = 0.80
trt.Override_trEfficiencyEndCapBArgon = 0.80

# Krypton thresholds (May 2015 - initial implementation) TRT_Digitization-01-01-00
# 0.000140 0.000150 0.003300 0.003000 0.003200 0.003100
trt.Override_lowThresholdBarKrypton        = 0.000140
trt.Override_lowThresholdECKrypton         = 0.000150
trt.Override_highThresholdBarShortKrypton  = 0.003300
trt.Override_highThresholdBarLongKrypton   = 0.003000
trt.Override_highThresholdECAwheelsKrypton = 0.003200
trt.Override_highThresholdECBwheelsKrypton = 0.003100
# TR efficiency for Krypton, defaults = 0.60, 0.90
trt.Override_trEfficiencyBarrelKrypton = 0.60
trt.Override_trEfficiencyEndCapAKrypton = 0.90
trt.Override_trEfficiencyEndCapBKrypton = 0.90

# T0, Old defaults = 7.0, 0.0
# T0, New defaults = 1.0, 0.0
# Note: if you change this from the default then you need to
#       set ToolSvc.InDetTRT_DriftFunctionTool.MCTuningShift
trt.Override_overallT0Shift            = 1.0
trt.Override_overallT0ShiftShortBarrel = 0.0

# Noise, defaults = 1, 0.02
trt.Override_noiseInUnhitStraws = 1
trt.Override_averageNoiseLevel  = 0.02

# HL delta shift w.r.t m_overallT0Shift (steps of 0.78125 ns) (HT middle-bit fraction tune)
# KyungEon.Choi@cern.ch https://indico.cern.ch/event/389682/contribution/5/material/slides/0.pdf
trt.Override_htT0shiftBarShort  = -6
trt.Override_htT0shiftBarLong   = -6
trt.Override_htT0shiftECAwheels = -6
trt.Override_htT0shiftECBwheels = -6

## EOF
