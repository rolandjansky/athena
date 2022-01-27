
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAactsvtx_preinclude.py" ) 

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

getInDetTrigConfig("jet")._actsVertex = True

log.info( "ID Trigger actsVertex:  "+str(getInDetTrigConfig("jet").actsVertex) )



