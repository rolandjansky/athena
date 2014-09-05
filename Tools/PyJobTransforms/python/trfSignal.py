# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfSignal
#
# @brief Signal handling utilities for ATLAS job transforms
# @details Signal handling for ATLAS job transforms
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfSignal.py 609252 2014-07-29 16:20:33Z wbreaden $
# 

import signal
import unittest

import logging
msg = logging.getLogger(__name__)

_defaultSignalHandlerDict = {}

## @brief Install common handler for various signals.
#  @details All existing signal handlers are saved in the @em _trfSignalHandlerDict dictionary to allow for them to be restored.
#  @param handler Common signal handler for all signals concerned.
#  @return None
def setTrfSignalHandlers(handler):
    xDict = {}
    if not _defaultSignalHandlerDict: # default handlers have not been saved
        xDict = _defaultSignalHandlerDict
    # N.B. Change from old transforms - do not catch SIGINT (^C). This is delt with more cleanly via
    # the stdTrfExceptionHandler wrapper which handles the KeyboardInterrupt exception.
    for s in ['SIGABRT', 'SIGFPE', 'SIGBUS', 'SIGHUP', 'SIGILL', 'SIGIO', 'SIGPIPE', 'SIGQUIT', 'SIGSEGV', 'SIGSYS', 'SIGXCPU', 'SIGXFSZ']:
        try:
            msg.debug("Setting signalhandler for %s to %s" % (s, handler))
            xDict[s] =  signal.signal(getattr(signal, s), handler)
        except Exception, e:
            msg.error("Unable to attach custom signal handler to %s: %s" % (s, e))
            continue
    return xDict

## @brief Restore signal handlers to the default ones
#  @details Handlers are restored from @em _defaultSignalHandlerDict dictionary.
#  @warning Signal handling in Python is not 100% implemented. Certain signal (even those listed below) cannot be caught.
#  @return None
def resetTrfSignalHandlers():
    if _defaultSignalHandlerDict:
        currentTRFSignalHandler = signal.getsignal(signal.SIGTERM)
    else:
        currentTRFSignalHandler = signal.SIG_DFL
    currentTRFSignalHandler = signal.getsignal(signal.SIGTERM)
    for s in ['SIGABRT', 'SIGFPE', 'SIGBUS', 'SIGHUP', 'SIGILL', 'SIGIO', 'SIGPIPE', 'SIGQUIT', 'SIGSEGV', 'SIGSYS', 'SIGTERM', 'SIGXCPU', 'SIGXFSZ']:
        try:
            signal.signal(getattr(signal, s), _defaultSignalHandlerDict.get(s, signal.SIG_DFL))
        except Exception, e:
            msg.error("Unable to attach custom signal handler to %s: %s" % (s, e))
            continue
    return currentTRFSignalHandler
