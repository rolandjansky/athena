# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from testsuitetest import Test, SummaryFileTest
import os.path

def run():
    for klass in [XMLFileTest]:
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

    def testHasThreeJobsTotal(self):
        return self.hasNJobs(3)

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================

