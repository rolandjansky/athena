# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time
# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

class PhasePrinter:
    def __init__(self):
        self.phase     = None
        self.startTime = 0

    def printIt(self, phase):
        endTime = time.time()
        if self.phase:
            exeTime = endTime-self.startTime
            logger.warning('\n================ %s ends, time = %d =============\n'   % (self.phase, exeTime))
            
            logger.warning('\n================ %s starts =============\n' % phase)
        self.phase = phase
        self.startTime = endTime

