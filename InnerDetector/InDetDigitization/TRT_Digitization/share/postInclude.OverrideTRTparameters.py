#########################################################
#                                                  v10  #
#                                                       #
# TRT_Digitization/postInclude.OverrideTRTparameters.py #
#                                                       #
# Overrride default settings in TRTDigSettings.cxx      #
#                                                       #
# This is not a full list, just the parameters likely   #
# to be used in basic tuning/FastOR studies.            #
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
   for alg in job:
      if hasattr(alg, 'PileUpTools'):
         trt = alg.PileUpTools[ "TRTDigitizationTool" ]
         break
      pass
if None == trt:
   raise AttributeError("TRTDigitization(Tool) not found.")

# Units are CLHEP: MeV, ns, mm

# Xenon thresholds
# Old  defaults = 0.000285, 0.000300, 0.006710, 0.006450, 0.006720, 0.006830 10-73
# New1 defaults = 0.000260, 0.000275, 0.006039, 0.005805, 0.006048, 0.006147 10-74-14 (with attenuation)
# New2 defaults = 0.000260, 0.000275, 0.006047, 0.005477, 0.005916, 0.005680 11-07    (fine-tune of HT)
trt.Override_lowThresholdBar        = 0.000260
trt.Override_lowThresholdEC         = 0.000275
trt.Override_highThresholdBarShort  = 0.006047
trt.Override_highThresholdBarLong   = 0.005477
trt.Override_highThresholdECAwheels = 0.005916
trt.Override_highThresholdECBwheels = 0.005680
# TR efficiency for Xenon, defaults = 0.95, 1.00
trt.Override_trEfficiencyBarrel = 0.95
trt.Override_trEfficiencyEndCap = 1.00

# Argon thresholds (July 2014 tuning from Artem) TRT_Digitization-01-00-09
# Defaults = 0.000070 0.000070 0.002446 0.002141 0.002235 0.002146
trt.Override_lowThresholdBarArgon        = 0.000070
trt.Override_lowThresholdECArgon         = 0.000070
trt.Override_highThresholdBarShortArgon  = 0.002446
trt.Override_highThresholdBarLongArgon   = 0.002141
trt.Override_highThresholdECAwheelsArgon = 0.002235
trt.Override_highThresholdECBwheelsArgon = 0.002146
# TR efficiency for Argon, defaults = 0.55, 1.80
trt.Override_trEfficiencyBarrel = 0.55
trt.Override_trEfficiencyEndCap = 0.80

# T0, Old defaults = 7.0, 0.0
# T0, New defaults = 1.0, 0.0
# Note: if you change this from the default then you need to
#       set ToolSvc.InDetTRT_DriftFunctionTool.MCTuningShift
trt.Override_overallT0Shift            = 1.0
trt.Override_overallT0ShiftShortBarrel = 0.0

# Noise, defaults = 1, 0.02
trt.Override_noiseInUnhitStraws = 1
trt.Override_averageNoiseLevel  = 0.02

# FastOR, defaults = 0(false), 0(false), 1.0 MeV.
trt.Override_doFastOr           = 0
trt.Override_doStreamer         = 0
trt.Override_streamerThreshold  = 1.0

## EOF
