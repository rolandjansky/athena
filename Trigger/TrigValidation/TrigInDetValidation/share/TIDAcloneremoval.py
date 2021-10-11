# leave this here for comparison ...
# ftf = findAlgorithm(topSequence, "TrigFastTrackFinder__electron")
# ftf.doCloneRemoval = True

from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAcloneremoval.py" ) 

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

getInDetTrigConfig("electron")._doCloneRemoval = False

log.info( "Setting clone removal: "+str(getInDetTrigConfig("electron").doCloneRemoval) )


