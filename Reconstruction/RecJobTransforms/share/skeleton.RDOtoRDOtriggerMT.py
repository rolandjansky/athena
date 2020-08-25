# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Transform skeleton for RDO -> RDO_TRIG (running trigger and adding its output to the RDO file).
# This is only a wrapper interfacing transform arguments into the main job options file
# TriggerJobOpts/runHLT_standalone.py
#

from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags

log = logging.getLogger('skeleton.RDOtoRDOtriggerMT')

##################################################
# Options read by runHLT_standalone, can be overwritten in runArgs/preExec/preInclude
##################################################
setMenu           = 'LS2_v1'
doWriteRDOTrigger = True
doWriteBS         = False

##################################################
# Parse runArgs
##################################################
if 'runArgs' not in globals():
    raise RuntimeError('runArgs not defined')

ra = globals()['runArgs']
def getFromRunArgs(propName, failIfMissing=True):
    if not hasattr(ra, propName):
        if failIfMissing:
            raise RuntimeError(propName + ' not defined in runArgs')
        else:
            return None
    return getattr(ra, propName)

# Input/Output
athenaCommonFlags.FilesInput = getFromRunArgs('inputRDOFile')
ConfigFlags.Input.Files = getFromRunArgs('inputRDOFile')
ConfigFlags.Output.RDOFileName = getFromRunArgs('outputRDO_TRIGFile')

# Max/skip events
maxEvents = getFromRunArgs('maxEvents', False) or -1
skipEvents = getFromRunArgs('skipEvents', False) or 0
athenaCommonFlags.EvtMax = maxEvents
athenaCommonFlags.SkipEvents = skipEvents

##################################################
# Parse preExec / preInclude
##################################################
preExec = getFromRunArgs('preExec', False)
if preExec:
    log.info('Executing transform preExec')
    for cmd in preExec:
        log.info(cmd)
        exec(cmd)

preInclude = getFromRunArgs('preInclude', False)
if preInclude:
    log.info('Executing transform preInclude')
    for fragment in preInclude:
        include(fragment)

##################################################
# Include the main job options
##################################################
include("TriggerJobOpts/runHLT_standalone.py")

##################################################
# Parse postExec / postInclude
##################################################
postExec = getFromRunArgs('postExec', failIfMissing=False)
if postExec:
    log.info('Executing transform postExec')
    for cmd in postExec:
        log.info(cmd)
        exec(cmd)

postInclude = getFromRunArgs('postInclude', failIfMissing=False)
if postInclude:
    log.info('Executing transform postInclude')
    for fragment in postInclude:
        include(fragment)
