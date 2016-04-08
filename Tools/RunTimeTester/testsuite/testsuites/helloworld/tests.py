# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from testsuitetest import Test, SummaryFileTest
import os.path

def run():
    for klass in [XMLFileTest, LogTest]:
        yield klass

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================

class XMLFileTest(SummaryFileTest):
    def __init__(self, tester):
        SummaryFileTest.__init__(self, tester)

    def testHasExactlyOnePackage(self):
        return self.hasExactlyNPackages(1)

    def testHasPackageCalled(self):
	return self.hasOnlyOnePackageCalled('AthExHelloWorld')

    def testHasOneJobOnly(self):
        return self.hasNJobs(1)

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================

class LogTest(Test):
    def __init__(self, tester):
        Test.__init__(self, tester)

    def testLogFileExists(self):
        minder = self.tester.testRuns[0].jobMinders[0]
        logfile = os.path.join(minder.resPath, minder.log)
        return os.path.exists(logfile)

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================


