
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAnopid.py" ) 

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

getInDetTrigConfig("electron")._electronPID = False

log.info( "Setting electronPID in the TrackSumaryTool: "+str(getInDetTrigConfig("electron").electronPID) )


