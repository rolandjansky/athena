# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfMPTools
#
# @brief Utilities for handling AthenaMP jobs
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfMPTools.py 731249 2016-03-19 22:05:45Z graemes $
# 

__version__ = '$Revision'

import os
import os.path as path
import re

import logging
msg = logging.getLogger(__name__)

from xml.etree import ElementTree

from PyJobTransforms.trfExitCodes import trfExit

import PyJobTransforms.trfExceptions as trfExceptions

## @brief Detect if AthenaMP has been requested
#  @param argdict Argument dictionary, used to access athenaopts for the job
#  @return Integer with the number of processes, N.B. 0 means non-MP serial mode
def detectAthenaMPProcs(argdict = {}):
    athenaMPProcs = 0
    
    # Try and detect if any AthenaMP has been enabled 
    try:
        if 'ATHENA_PROC_NUMBER' in os.environ:
            athenaMPProcs = int(os.environ['ATHENA_PROC_NUMBER'])
            if athenaMPProcs < 0:
                raise ValueError("ATHENA_PROC_NUMBER value was less than zero")
            msg.info('AthenaMP detected from ATHENA_PROC_NUMBER with {0} workers'.format(athenaMPProcs))
        elif 'athenaopts' in argdict:
            for substep in argdict['athenaopts'].value:
                procArg = [opt.replace("--nprocs=", "") for opt in argdict['athenaopts'].value[substep] if '--nprocs' in opt]
                if len(procArg) == 0:
                    athenaMPProcs = 0
                elif len(procArg) == 1:
                    athenaMPProcs = int(procArg[0])
                    if athenaMPProcs < 0:
                        raise ValueError("--nprocs was set to a value less than zero")
                else:
                    raise ValueError("--nprocs was set more than once in 'athenaopts'")
                msg.info('AthenaMP detected from "nprocs" setting with {0} workers for substep {1}'.format(athenaMPProcs,substep))
    except ValueError, errMsg:
        myError = 'Problem discovering AthenaMP setup: {0}'.format(errMsg)
        raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), myError)

    return athenaMPProcs

## @brief Handle AthenaMP outputs, updating argFile instances to real 
#  @param athenaMPFileReport XML file with outputs that AthenaMP knew about
#  @param athenaMPWorkerTopDir Subdirectory with AthenaMP worker run directories
#  @param dataDictionary This substep's data dictionary, allowing all files to be
#  updated to the appropriate AthenaMP worker files
#  @param athenaMPworkers Number of AthenaMP workers
#  @return @c None; side effect is the update of the @c dataDictionary
def athenaMPOutputHandler(athenaMPFileReport, athenaMPWorkerTopDir, dataDictionary, athenaMPworkers):
    msg.debug("MP output handler called for report {0} and workers in {1}, data types {2}".format(athenaMPFileReport, athenaMPWorkerTopDir, dataDictionary.keys()))
    outputHasBeenHandled = dict([ (dataType, False) for dataType in dataDictionary.keys() if dataDictionary[dataType] ])
    
    # First, see what AthenaMP told us
    mpOutputs = ElementTree.ElementTree()
    try:
        mpOutputs.parse(athenaMPFileReport)
    except IOError:
        raise trfExceptions.TransformExecutionException(trfExit.nameToCode("TRF_OUTPUT_FILE_ERROR"), "Missing AthenaMP outputs file {0} (probably athena crashed)".format(athenaMPFileReport))
    for filesElement in mpOutputs.getroot().getiterator(tag='Files'):
        msg.debug('Examining element {0} with attributes {1}'.format(filesElement, filesElement.attrib))
        originalArg = None 
        originalName = filesElement.attrib['OriginalName']
        for dataType, fileArg in dataDictionary.iteritems():
            if fileArg.value[0] == originalName:
                originalArg = fileArg
                outputHasBeenHandled[dataType] = True
                break
        if originalArg is None:
            msg.warning('Found AthenaMP output with name {0}, but no matching transform argument'.format(originalName))
            continue
        
        msg.debug('Found matching argument {0}'.format(originalArg))
        fileNameList = []
        for fileElement in filesElement.getiterator(tag='File'):
            msg.debug('Examining element {0} with attributes {1}'.format(fileElement, fileElement.attrib))
            fileNameList.append(path.relpath(fileElement.attrib['name']))
            
        athenaMPoutputsLinkAndUpdate(fileNameList, fileArg) 
        
    # Now look for additional outputs that have not yet been handled
    if len([ dataType for dataType in outputHasBeenHandled if outputHasBeenHandled[dataType] is False]):
        # OK, we have something we need to search for; cache the dirwalk here
        MPdirWalk = [ dirEntry for dirEntry in os.walk(athenaMPWorkerTopDir) ]
        
        for dataType, fileArg in dataDictionary.iteritems():
            if outputHasBeenHandled[dataType]:
                continue
            if fileArg.io is "input":
                continue
            msg.info("Searching MP worker directories for {0}".format(dataType))
            originalName = fileArg.value[0]
            fileNameList = []
            for entry in MPdirWalk:
                if "evt_count" in entry[0]:
                    continue
                # N.B. AthenaMP may have made the output name unique for us, so 
                # we need to treat the original name as a prefix
                possibleOutputs = [ fname for fname in entry[2] if fname.startswith(originalName) ]
                if len(possibleOutputs) == 0:
                    continue
                elif len(possibleOutputs) == 1:
                    fileNameList.append(path.join(entry[0], possibleOutputs[0]))
                else:
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode("TRF_OUTPUT_FILE_ERROR"), "Found multiple matching outputs for datatype {0} in {1}: {2}".format(dataType, entry[0], possibleOutputs))
            if len(fileNameList) != athenaMPworkers:
                raise trfExceptions.TransformExecutionException(trfExit.nameToCode("TRF_OUTPUT_FILE_ERROR"), "Found {0} output files for {1}, expected {2} (found: {3})".format(len(fileNameList), dataType, athenaMPworkers, fileNameList))
            
            # Found expected number of files - good!
            athenaMPoutputsLinkAndUpdate(fileNameList, fileArg) 


def athenaMPoutputsLinkAndUpdate(newFullFilenames, fileArg):
    originalName = fileArg.value[0]
    # Do we need to append worker dir suffixes?
    linkedNameList = []
    uniqueSimpleNames = set([path.basename(fname) for fname in newFullFilenames])
    # Check here if MP created it's own unique names - otherwise we need to add suffixes
    # so that each output file is unique
    if len(uniqueSimpleNames) != len(newFullFilenames):
        indexesUsed = []
        for fname in newFullFilenames:
            simpleName = path.basename(fname)
            workerIndexMatch = re.search(r'/worker_(\d+)/', fname)
            if workerIndexMatch:
                fileIndex = int(workerIndexMatch.group(1)) + 1
            else:
                fileIndex = 0
            if fileIndex in indexesUsed:
                fileIndex = max(indexesUsed)+1
            indexesUsed.append(fileIndex)
            simpleName += "_{0:03d}".format(int(fileIndex))
            linkedNameList.append(simpleName)
    else:
        linkedNameList = [path.basename(fname) for fname in newFullFilenames]

    for linkname, fname in zip(linkedNameList, newFullFilenames):
        try:
            if path.lexists(linkname):
                os.unlink(linkname)
            os.symlink(fname, linkname)
        except OSError, e:  
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode("TRF_OUTPUT_FILE_ERROR"), "Failed to link {0} to {1}: {2}".format(fname, linkname, e))

    fileArg.multipleOK = True
    fileArg.value = linkedNameList
    fileArg.originalName = originalName
    msg.debug('MP output argument updated to {0}'.format(fileArg))
    
