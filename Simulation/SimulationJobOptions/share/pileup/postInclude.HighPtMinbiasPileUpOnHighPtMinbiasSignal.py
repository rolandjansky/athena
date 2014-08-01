#########################################################
#
# SimulationJobOptions/postOptions.DDMinbiasPileUpOnDDMinbiasSignal.py
# John Chapman
#
# Correct the amount of DD minbias pile-up in BC0 to account for
# the fact that there is already a minbias event (the
# signal event) in that bunch crossing.
#
# This job option should be added via the postInclude
# command line argument. 
#
#########################################################

from Digitization.DigitizationFlags import digitizationFlagsh
if hasattr(digitizationFlags, 'doHighPtMinBias'): 
    from AthenaCommon.AppMgr import ServiceMgr
    if hasattr(ServiceMgr, 'PileUpEventLoopMgr'):
        if digitizationFlags.doHighPtMinBias: 
            ServiceMgr.PileUpEventLoopMgr.bkgCaches["HighPtMinBiasCache"].SubtractBC0 = 1
    else:
        raise AttributeError("This fragment should only be used in pile-up jobs!")
else:
    raise AttributeError("digitizationFlags has no doHighPtMinBias property! This postInclude file is only for use with 17.X.Y or later releases!")
