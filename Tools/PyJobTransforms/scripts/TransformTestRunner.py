#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
## @note Find a named test in the DATAPATH and execute it with all arguments
#  @version $Id: TransformTestRunner.py 551981 2013-06-21 10:16:16Z graemes $

import os
import os.path
import sys

from PyJobTransforms.trfUtils import findFile
from PyJobTransforms.trfLogger import msg

def main():
    if len(sys.argv) < 2:
        msg.error('No test argument was given')
        sys.exit(1)
        
    if 'DATAPATH' not in os.environ:
        msg.error('There is no DATAPATH to search along - is the release setup?')
        sys.exit(1)
    
    testScript = os.path.join('JobTransforms/test', sys.argv[1])
    
    pathToTestScript = findFile(os.environ['DATAPATH'], testScript)
    if pathToTestScript is None:
        msg.error('Test {0} was not found along DATAPATH'.format(testScript))
        sys.exit(1)
    
    msg.info('Found test {0} here: {1}'.format(sys.argv[1], pathToTestScript))
    os.execv(pathToTestScript, sys.argv[1:])

if __name__ == '__main__':
    main()