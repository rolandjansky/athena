######################################################################################
#
# Skeleton job options to run
# TrigInDetValidation/TrigInDetValidation_RTT_topOptions.py 
# in ATN to catch errors early
#
######################################################################################

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

acf.EvtMax=2
if not acf.EvtMax.is_locked():
    acf.EvtMax.set_Value_and_Lock(2)
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

#-------------------------------------------------------------------
include("TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py")
#-----------------------------------------------------------

#-------------------------------------------------------------------
include("TriggerTest/TriggerTestCommon.py")
#-----------------------------------------------------------


