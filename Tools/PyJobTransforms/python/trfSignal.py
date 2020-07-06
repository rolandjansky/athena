# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfSignal
#
# @brief Signal handling utilities for ATLAS job transforms
# @details Signal handling for ATLAS job transforms
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfSignal.py 623865 2014-10-24 12:39:44Z graemes $
# 

import signal

import logging
msg = logging.getLogger(__name__)

_savedSignalHandlerDict = {}

# N.B. Do not catch SIGINT (^C). This is delt with more cleanly via
# the stdTrfExceptionHandler wrapper which handles the KeyboardInterrupt exception.
# Also, don't intercept SIGTERM as this is used by the multiprocessing module to
# terminate child proceses
_defaultSignalList = ['SIGABRT', 'SIGFPE', 'SIGBUS', 'SIGHUP', 'SIGILL', 'SIGIO', 'SIGPIPE', 'SIGQUIT', 'SIGSEGV', 'SIGSYS', 'SIGXCPU', 'SIGXFSZ']

## @brief Install common handler for various signals.
#  @details All existing signal handlers are saved in the @em _trfSignalHandlerDict dictionary to allow for them to be restored.
#  @param handler Common signal handler for all signals concerned.
#  @return None
def setTrfSignalHandlers(handler):
    for s in _defaultSignalList:
        try:
            msg.debug("Setting signalhandler for %s to %s", s, handler)
            _savedSignalHandlerDict[s] =  signal.signal(getattr(signal, s), handler)
        except Exception as e:
            msg.error("Unable to attach custom signal handler to %s: %s", s, e)
            continue

## @brief Restore signal handlers to the default ones
#  @details Handlers are restored from @em _defaultSignalHandlerDict dictionary.
#  @return None
def resetTrfSignalHandlers():
    for s in _defaultSignalList:
        try:
            signal.signal(getattr(signal, s), _savedSignalHandlerDict.get(s, signal.SIG_DFL))
        except Exception as e:
            msg.error("Unable to attach custom signal handler to %s: %s", s, e)
            continue
