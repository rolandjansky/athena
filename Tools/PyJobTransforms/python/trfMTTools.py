# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfMTTools
#
# @brief Utilities for handling AthenaMT jobs
# 

__version__ = '$Revision'

import os

import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfExitCodes import trfExit

import PyJobTransforms.trfExceptions as trfExceptions

## @brief Detect if AthenaMT has been requested
#  @param argdict Argument dictionary, used to access athenaopts for the job
#  @return Two integers with the number of threads and number of concurrent events, N.B. 0 means non-MT serial mode
def detectAthenaMTThreads(argdict = {}):
    athenaMTThreads = 0
    athenaConcurrentEvents = 0

    # Try and detect if any AthenaMT has been enabled 
    try:
        if 'athenaopts' in argdict:
            for substep in argdict['athenaopts'].value:
                threadArg = [opt.replace("--threads=", "") for opt in argdict['athenaopts'].value[substep] if '--threads' in opt]
                if len(threadArg) == 0:
                    athenaMTThreads = 0
                elif len(threadArg) == 1:
                    if 'multithreaded' in argdict:
                        raise ValueError("Detected conflicting methods to configure AthenaMT: --multithreaded and --threads=N (via athenaopts). Only one method must be used")
                    athenaMTThreads = int(threadArg[0])
                    if athenaMTThreads < -1:
                        raise ValueError("--threads was set to a value less than -1")
                else:
                    raise ValueError("--threads was set more than once in 'athenaopts'")
                msg.info('AthenaMT detected from "threads" setting with {0} threads for substep {1}'.format(athenaMTThreads,substep))

                concurrentEventsArg = [opt.replace("--concurrent-events=", "") for opt in argdict['athenaopts'].value[substep] if '--concurrent-events' in opt]
                if len(concurrentEventsArg) == 1:
                    athenaConcurrentEvents = int(concurrentEventsArg[0])
                    if athenaConcurrentEvents < -1:
                        raise ValueError("--concurrent-events was set to a value less than -1")
                    msg.info('Custom concurrent event setting read from "concurrent-events" with {0} events for substep {1}'.format(athenaConcurrentEvents,substep))
                else:
                    athenaConcurrentEvents = athenaMTThreads
        if (athenaMTThreads == 0 and
            'ATHENA_CORE_NUMBER' in os.environ and
            'multithreaded' in argdict):
            athenaMTThreads = int(os.environ['ATHENA_CORE_NUMBER'])
            if athenaMTThreads < -1:
                raise ValueError("ATHENA_CORE_NUMBER value was less than -1")
            msg.info('AthenaMT detected from ATHENA_CORE_NUMBER with {0} threads'.format(athenaMTThreads))
            athenaConcurrentEvents = athenaMTThreads
    except ValueError as errMsg:
        myError = 'Problem discovering AthenaMT setup: {0}'.format(errMsg)
        raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), myError)

    return athenaMTThreads, athenaConcurrentEvents
