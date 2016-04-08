# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from summaryFile import RTTSummaryFile
from stats import TestStatistics

class Test:
    def __init__(self, tester):
        self.tester      = tester
        self.summaryFile = RTTSummaryFile(tester.xmlConverter.RTTSummary)
        self.stats       = TestStatistics(self.__class__.__name__)

    def run(self): 
        for testcase in [getattr(self, t) for t in dir(self.__class__) if t.startswith('test') and callable(getattr(self,t))]:
            retvalue = None
            try:
                retvalue = testcase()
            except Exception, e:
                self.stats += (testcase.__name__, False, 'Test code broken? Raised an exception: %s' % str(e))
            else:            
                self.stats += (testcase.__name__, retvalue, '')

class SummaryFileTest(Test):
    def __init__(self, tester):
	Test.__init__(self, tester)

    def hasExactlyNPackages(self, n): return len(self.summaryFile.packages) == n

    def hasOnlyOnePackageCalled(self, name):
        return len([p for p in self.summaryFile.packages if p.name==name])==1

    def hasAtLeastNPackages(self, npacks):
	return len(self.summaryFile.packages)>=npacks

    def hasNJobs(self, njobs):
        return reduce(lambda x,y:x+y, [len(p.jobs) for p in self.summaryFile.packages], 0) == njobs

    def testHasOnlySuccessJobs(self):
        return False not in [p.hasOnlySuccessJobs() for p in self.summaryFile.packages]
