# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfFileValidationFunctions
# @brief Transform file validation functions
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfUtils.py 578615 2014-01-15 21:22:05Z wbreaden $

import logging
msg = logging.getLogger(__name__)

## @brief Integrity function for file class argPOOLFile, argHITSFile and argRDOFile
def returnIntegrityOfPOOLFile(file):
    from PyJobTransforms.trfValidateRootFile import checkFile
    rc = checkFile(fileName = file, type = 'event', requireTree = True)
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(file)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(file), integrityStatus = rc))

## @brief Integrity function for file class argNTUPFile
def returnIntegrityOfNTUPFile(file):
    from PyJobTransforms.trfValidateRootFile import checkFile
    rc = checkFile(fileName = file, type = 'basket', requireTree = False)
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(file)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(file), integrityStatus = rc))

## @brief Integrity function for file class argBSFile
def returnIntegrityOfBSFile(file):
    try:
        from PyJobTransforms.trfUtils import call
        rc = call(["AtlListBSEvents.exe", "-c", file],
            logger = msg,
            message = "Report by AtlListBSEvents.exe: ",
            timeout = None
        )
    except trfExceptions.TransformTimeoutException:
        return False
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(file)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(file), integrityStatus = rc))

### @brief Integrity function for file class argTAGFile
def returnIntegrityOfTAGFile(file):
    from PyJobTransforms.trfFileUtils import AthenaFileInfo
    dictionaryOfAthenaFileInfo = AthenaFileInfo([str(file),], retrieveKeys = ['nentries',])
    msg.debug("dictionary of Athena file information: {a}".format(a = dictionaryOfAthenaFileInfo))
    eventCount = dictionaryOfAthenaFileInfo[str(file)]['nentries']
    if eventCount is None:
        return (False, "integrity of {fileName} bad: got a bad event count in {fileName}: {eventCount}".format(fileName = str(file), eventCount = eventCount))
    else:
        return (True, "integrity of {fileName} good".format(fileName = str(file)))

## @brief Integrity function for file class argHISTFile
def returnIntegrityOfHISTFile(file):
    from PyJobTransforms.trfValidateRootFile import checkFile
    rc = 0 # (default behaviour)
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(file)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(file), integrityStatus = rc))
