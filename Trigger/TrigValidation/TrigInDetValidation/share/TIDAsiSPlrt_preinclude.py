
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAsiSPlrt_preinclude.py" ) 

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doR3LargeD0.set_Value_and_Lock(True)
InDetFlags.storeSeparateLargeD0Container.set_Value_and_Lock(False)  


from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

getInDetTrigConfig("muonLRT")._useSiSPSeededTrackFinder = True

log.info( "ID Trigger muonLRT useSiSPSeededTrackFinder = True")


