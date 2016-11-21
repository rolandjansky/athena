# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfFileValidationFunctions
# @brief Transform file validation functions
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfUtils.py 578615 2014-01-15 21:22:05Z wbreaden $

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfExceptions as trfExceptions

## @brief Integrity function for file class argPOOLFile, argHITSFile, argRDOFile and argEVNTFile
def returnIntegrityOfPOOLFile(fname):
    from PyJobTransforms.trfValidateRootFile import checkFile
    rc = checkFile(fileName = fname, type = 'event', requireTree = True)
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(fname)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(fname), integrityStatus = rc))

## @brief Integrity function for file class argNTUPFile
def returnIntegrityOfNTUPFile(fname):
    from PyJobTransforms.trfValidateRootFile import checkFile
    rc = checkFile(fileName = fname, type = 'basket', requireTree = False)
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(fname)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(fname), integrityStatus = rc))

## @brief Integrity function for file class argBSFile
def returnIntegrityOfBSFile(fname):
    try:
        from PyJobTransforms.trfUtils import call
        rc = call(["AtlListBSEvents.exe", "-c", fname],
            logger = msg,
            message = "Report by AtlListBSEvents.exe: ",
            timeout = None
        )
    except trfExceptions.TransformTimeoutException:
        return False
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(fname)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(fname), integrityStatus = rc))

### @brief Integrity function for file class argTAGFile
def returnIntegrityOfTAGFile(fname):
    from PyJobTransforms.trfFileUtils import AthenaLiteFileInfo
    dictionaryOfAthenaFileInfo = AthenaLiteFileInfo(fname, "TAG", retrieveKeys = ['nentries',])
    msg.debug("dictionary of Athena file information: {a}".format(a = dictionaryOfAthenaFileInfo))
    eventCount = dictionaryOfAthenaFileInfo[fname]['nentries']
    if eventCount is None:
        return (False, "integrity of {fileName} bad: got a bad event count in {fileName}: {eventCount}".format(fileName = str(fname), eventCount = eventCount))
    else:
        return (True, "integrity of {fileName} good".format(fileName = str(fname)))

## @brief Integrity function for file class argHISTFile
def returnIntegrityOfHISTFile(fname):
    rc = 0 # (default behaviour)
    if rc == 0:
        return (True, "integrity of {fileName} good".format(fileName = str(fname)))
    else:
        return (False, "integrity of {fileName} bad: return code: {integrityStatus}".format(fileName = str(fname), integrityStatus = rc))
