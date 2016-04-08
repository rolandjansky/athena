# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'a glue function to allow running a testsuite via popen - ie as subprocess'

import sys
from testsuite import TestSuite
def doit():
    path = sys.argv[1]
    ts = TestSuite(path)
    ts.run()
    
if __name__ == '__main__':
    doit()
