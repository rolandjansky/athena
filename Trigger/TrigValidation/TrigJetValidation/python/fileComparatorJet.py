#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
fileComparatorJet this module contains:
functions:
fileComparatorJet   - function to compare two files, and write out a diff log
                   if there are differences.
"""


######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

import os
import filecmp
import sys

sys.path.append(os.path.dirname(sys.modules[__name__].__file__))

from subprocess import (Popen,
                        PIPE)

def fileComparatorJet(testFile, refFile):
    """Compare two files. Return 0 if same, 1 otherwise.
    Otherwise raise an exception. The RTT runs post processor tests
    through RTTTestRunner which handles results and exceptions."""

    print 'Running simple file comparator'

    for f in (testFile, refFile):
        if not os.path.exists(f):
            m = 'fileComparator: file %s does not exist, giving up' % (f)
            print m
            raise RuntimeError(m)

    if filecmp.cmp(testFile, refFile):
        return 0

    diffCommand = [
        'diff %s %s' % (testFile, refFile)
        ]

    po = Popen(diffCommand,
               shell=True,
               stdout=PIPE,
               stderr=PIPE)
    out, err = po.communicate()

    # if the files were the same, filecmp above would have succeeded
    # and returned 0. So if we are here, it's because the files differ.
    # Which means code == 1. If not 1, then problem running the popen,
    # so we let that exception be raised.
    assert po.returncode == 1

    print 'fileComparator found files %s %s to differ' % (testFile, refFile)
    print out
    print err
    return 1

if __name__ == '__main__':
    testFile = sys.argv[1]
    refFile = sys.argv[2]
    sys.exit(fileComparatorJet(testFile, refFile))
