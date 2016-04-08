# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import os
def runTestCase(TestCase):
    suite = unittest.makeSuite(TestCase, 'test')
    result = unittest.TestResult()
    suite.run(result)
    suitePath = os.getcwd()
    suiteName = os.path.basename(suitePath)
    ntests    = result.testsRun
    nerrors   = len(result.errors)
    nfailures = len(result.failures)
    
    if result.errors:
        of = open(os.path.join(suitePath, 'TestCaseErrors.log'), 'w')
        [of.write(t[1]) for t in result.errors]
        
    if result.failures:
        of = open(os.path.join(suitePath, 'TestCaseFailures.log'), 'w')
        [of.write(t[1]) for t in result.failures]
        
    if result.wasSuccessful():
        ok = 'OK'
    else:
        ok = 'check %s ' % os.path.join(suiteName,'TestCase.log')
    print '%s Errors/Failures/Successes    %d %d %d %s' % (suiteName.ljust(30), nerrors, nfailures, (ntests-nerrors-nfailures), ok)
