
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAtaupt_preinclude.py" ) 

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from AthenaCommon.SystemOfUnits import GeV

getInDetTrigConfig("tauIso")._pTmin = 0.8*GeV

log.info( "ID Trigger pTmin: "+str(getInDetTrigConfig("tauIso").pTmin) )


