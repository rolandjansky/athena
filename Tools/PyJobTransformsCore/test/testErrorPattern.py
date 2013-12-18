#!/usr/bin/env python
#
#  testErrorPattern.py
#  
#
#  Created by Alvin on 16/04/2009.
#  Copyright (c) 2009 University of Birmingham. All rights reserved.
#
import sys, os
from PyJobTransformsCore.trferr import AthenaLogChecker
from PyJobTransformsCore.VTimer import vTimer

usage = "testErrorPattern RELEASE ERROR_STRING"
#print len(sys.argv)
if len(sys.argv) != 3:
    print usage
    sys.exit(1)
release = sys.argv[1]
errorStr = sys.argv[2]
l = AthenaLogChecker( release )
if os.path.exists( os.path.expanduser( os.path.expandvars( errorStr ) ) ):
    errorFile = open( errorStr,'r')
else:
    # treat as string
    errorFile = [ errorStr ]

noError = True
vTimer.start()
for line in errorFile:
    result = l.processLine( line )
    if result and result.severity in [ 'ERROR', 'FATAL' ]:
        print "-->%s [%s: %s %s]\n" % ( line, result.severity, result.acronym, result.code )
        noError = False
vTimer.stop()
vTimer.reset()
if noError:
    print "No errors found."

sys.exit(0)
