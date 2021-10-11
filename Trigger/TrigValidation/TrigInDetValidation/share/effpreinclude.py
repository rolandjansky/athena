
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: effpreinclude.py" ) 


from TrigInDetConfig.ConfigSettingsBase import _ConfigSettingsBase 

def override(self):
    self._newConfig=True

_ConfigSettingsBase.override = override


# potential future check - leave here
# from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags ; InDetTrigFlags.cutLevel.set_Value_and_Lock(15) 

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from AthenaCommon.SystemOfUnits import GeV

getInDetTrigConfig("bjet")._pTmin   = 0.8*GeV
getInDetTrigConfig("tauIso")._pTmin = 0.8*GeV

log.info( "ID Trigger pTmin: "+str(getInDetTrigConfig("bjet").pTmin) )
log.info( "ID Trigger pTmin: "+str(getInDetTrigConfig("tauIso").pTmin) )



