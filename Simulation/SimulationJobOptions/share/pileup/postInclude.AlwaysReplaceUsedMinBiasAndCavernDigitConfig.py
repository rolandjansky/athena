#########################################################
#
# SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py
# John Chapman
#
# Increase the cache refresh rate to 100% for minbias and
# cavern background samples.
#
# This job option should be added via the postInclude
# command line argument.
#
# FIXME: Rename to a more descriptive name.
#
#########################################################

from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AppMgr import ServiceMgr
if hasattr(ServiceMgr, 'PileUpEventLoopMgr'):
    if hasattr(digitizationFlags, 'doMinimumBias'): 
        if digitizationFlags.doMinimumBias: 
            ServiceMgr.PileUpEventLoopMgr.bkgCaches["MinBiasCache"].ReadDownscaleFactor = 1 #default 150
    else:
        raise AttributeError("digitizationFlags has no doMinimumBias property! This postInclude file is only for use with 16.6.X releases!")
    if digitizationFlags.doCavern():
        ServiceMgr.PileUpEventLoopMgr.bkgCaches["CavernCache"].ReadDownscaleFactor = 1 #default 150        
else:
    raise AttributeError("This fragment should only be used in pile-up jobs!")

