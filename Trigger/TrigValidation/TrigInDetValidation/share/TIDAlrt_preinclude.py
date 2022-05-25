
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAlrt_preinclude.py" ) 

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doR3LargeD0.set_Value_and_Lock(True)
InDetFlags.storeSeparateLargeD0Container.set_Value_and_Lock(False) 

#ATR-25582 - FSLRT is now excluded from the default dev menu so need to change to the full dev menu rather than the filtered versions
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Trigger.triggerMenuSetup='Dev_pp_run3_v1'
