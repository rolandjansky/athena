#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
## to force batch mode for PyROOT
sys.argv.append("-b")

from DozerObject import *
from DozerUtils import *
from DozerIOV import *
from DozerData import *
from DozerChannel import *
from DozerSchema import *
from DozerSchemaConfig import *
from DozerConfig import *
from DozerRecordSet import *
from DozerGraph import *
from DozerDB import *

import unittest

## test suite execution
if __name__ == "__main__":
 
    log = DozerLogger("test_CoolDozerAPI.log", toConsole=True)
    globals()["fgjdfhgkjdfhg"] = log

    testLoader = unittest.TestLoader()
    suiteUtils   = testLoader.loadTestsFromTestCase( test_DozerUtils   )   
    suiteIOV     = testLoader.loadTestsFromTestCase( test_DozerIOV     )   
    suiteData    = testLoader.loadTestsFromTestCase( test_DozerData    )   
    suiteChannel = testLoader.loadTestsFromTestCase( test_DozerChannel )   
    suiteSchema  = testLoader.loadTestsFromTestCase( test_DozerSchema  )   
    suiteConfig  = testLoader.loadTestsFromTestCase( test_DozerConfig  )  
    suiteDB      = testLoader.loadTestsFromTestCase( test_DozerDB      )   
    suiteRecordSet = testLoader.loadTestsFromTestCase( test_DozerRecordSet )
    suiteGraph = testLoader.loadTestsFromTestCase( test_DozerGraph )

    alltests = unittest.TestSuite([ suiteUtils,
                                    suiteIOV,
                                    suiteData,
                                    suiteChannel,
                                    suiteSchema,
                                    suiteConfig,
                                    suiteDB,
                                    suiteRecordSet,
                                    suiteGraph ] )

    unittest.TextTestRunner(verbosity=2).run(alltests)



