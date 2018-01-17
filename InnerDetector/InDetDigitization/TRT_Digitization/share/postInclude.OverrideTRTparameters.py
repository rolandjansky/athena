#########################################################
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
# 0.000260, 0.000275, 0.005195, 0.004571, 0.005513, 0.005326   01-01-16    (Delta-ray suppression tune - 2015 data)
trt.Override_lowThresholdBar        = 0.000260
trt.Override_lowThresholdEC         = 0.000275
trt.Override_highThresholdBarShort  = 0.005195
trt.Override_highThresholdBarLong   = 0.004751
trt.Override_highThresholdECAwheels = 0.005513
trt.Override_highThresholdECBwheels = 0.005326
# TR efficiency for Xenon, defaults = 0.95, 1.00, 1.00
# 0.774, 0.932, 0.830 (Delta-ray suppression tune)
# 0.774, 0.909, 0.809 (Delta-ray suppression tune - 2015 data)
trt.Override_trEfficiencyBarrel = 0.774
trt.Override_trEfficiencyEndCapA = 0.909
trt.Override_trEfficiencyEndCapB = 0.809

# Argon thresholds (July 2014 tuning from Artem) TRT_Digitization-01-00-09
# 0.000070 0.000070 0.002446 0.002141 0.002235 0.002146   01-00-09
# 0.000070 0.000070 0.002660 0.002352 0.002414 0.002295   01-00-24    (HT middle-bit fraction tune - scaled from Xenon study)
# 0.000070 0.000070 0.002607 0.002540 0.002414 0.002295   01-01-16    (HT middle-bit fraction tune - 2015 data)
trt.Override_lowThresholdBarArgon        = 0.000150
trt.Override_lowThresholdECArgon         = 0.000150
trt.Override_highThresholdBarShortArgon  = 0.002607
trt.Override_highThresholdBarLongArgon   = 0.002540
trt.Override_highThresholdECAwheelsArgon = 0.002414
trt.Override_highThresholdECBwheelsArgon = 0.002295
# TR efficiency for Argon, defaults = 0.55, 0.80
# TR efficiency for Argon, defaults = 0.61, 0.80, 0.80 (HT middle-bit fraction tune - 2015 data)
trt.Override_trEfficiencyBarrelArgon = 0.61
trt.Override_trEfficiencyEndCapAArgon = 0.80
trt.Override_trEfficiencyEndCapBArgon = 0.80

# Krypton thresholds
# 0.000140 0.000150 0.003300 0.003000 0.003200 0.003100   01-01-00 (May 2015 - initial implementation)
# 0.000140 0.000150 0.003070 0.002900 0.003150 0.002970   01-02-01 (April 2016 tuning from 2015 data by Kevin, no LT tune)
trt.Override_lowThresholdBarKrypton        = 0.000140
trt.Override_lowThresholdECKrypton         = 0.000150
trt.Override_highThresholdBarShortKrypton  = 0.003070
trt.Override_highThresholdBarLongKrypton   = 0.002900
trt.Override_highThresholdECAwheelsKrypton = 0.003150
trt.Override_highThresholdECBwheelsKrypton = 0.003020
# TR efficiency for Krypton, defaults = 0.60, 0.90   
# TR efficiency for Krypton, defaults = 0.49, 0.68        01-02-01 (April 2016 tuning from 2015 data by Kevin)
trt.Override_trEfficiencyBarrelKrypton = 0.49
trt.Override_trEfficiencyEndCapAKrypton = 0.68
trt.Override_trEfficiencyEndCapBKrypton = 0.68

# Noise, defaults = 0(since July 2017), 0.02
trt.Override_noiseInUnhitStraws = 0
trt.Override_averageNoiseLevel  = 0.02

# HL shift (steps of 0.78125 ns) (HT middle-bit fraction tune)
# KyungEon.Choi@cern.ch https://indico.cern.ch/event/389682/contribution/5/material/slides/0.pdf
trt.Override_htT0shiftBarShort  = -6
trt.Override_htT0shiftBarLong   = -6
trt.Override_htT0shiftECAwheels = -6
trt.Override_htT0shiftECBwheels = -6

# LL shift (steps of 0.78125 ns) (Christophe Roland Argon shaping tuning)
# Note: if you change this from the default then you need to
#       set ToolSvc.InDetTRT_DriftFunctionTool.MCTuningShift
# Add these after the final Ar tune 
## EOF
