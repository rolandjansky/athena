#########################################################
#
# SimulationJobOptions/postOptions.NDMinbiasPileUpOnNDMinbiasSignal.py
# John Chapman
#
# Correct the amount of ND minbias pile-up in BC0 to account for
# the fact that there is already a minbias event (the
# signal event) in that bunch crossing.
#
# This job option should be added via the postInclude
# command line argument. 
#
#########################################################

from Digitization.DigitizationFlags import digitizationFlags
if hasattr(digitizationFlags, 'doNDMinimumBias'): 
    from AthenaCommon.AppMgr import ServiceMgr
    if hasattr(ServiceMgr, 'PileUpEventLoopMgr'):
        if digitizationFlags.doNDMinimumBias: 
            ServiceMgr.PileUpEventLoopMgr.bkgCaches["NDMinBiasCache"].SubtractBC0 = 1
    else:
        raise AttributeError("This fragment should only be used in pile-up jobs!")
else:
    raise AttributeError("digitizationFlags has no doNDMinimumBias property! This postInclude file is only for use with 16.6.X releases!")
