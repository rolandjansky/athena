# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from utility import log
from summaryFile import RTTSummaryFile
import os

def run():
    for klass in [XMLFileChecker]:
        yield klass

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================

class XMLFileChecker:
    def __init__(self, tester):
        self.tester   = tester
        self.summfile = RTTSummaryFile(self.tester.paths.RTTSummary)

    def run(self):
        self.hasZeroJobs()
        
    @log
    def hasZeroJobs(self):
        return len(self.summfile.packages[0].jobs) == 0

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================



