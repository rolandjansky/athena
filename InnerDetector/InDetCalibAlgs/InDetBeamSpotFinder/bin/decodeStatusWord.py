#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Convert the beamspot status-word into a meaningful output
"""

__author__  = 'James Walder'
__version__ = '$Id$'
__usage__   = '%prog [options] statusWord]'

import sys


def getStatus(stat):
    status = int(stat)
    suc =  ( status >> 0 ) & 3
    onl =  ( status >> 2 ) & 1
    wid =  ( status >> 3 ) & 1
    alg =  ( status >> 4 ) & 15
    print status,": ", suc, onl, wid, alg
    if status == 0:
        return ("Default used", False, False, None )
    if status == 83: # move this to a standard setting
        return ("Run average", False, False, "Averaging" )
    
    if onl == 0:
        online = False
    else:
        online = True
    if wid == 0:  
        fitWidth = False
    else:
        fitWidth = True
    if suc == 1:
        success = "Problems?"
    elif suc == 2:
        success = "Reserved"
    elif suc == 3:
        success = "Successful"
    elif suc == 0:
        success = "Failed?"
    else:
        success = "Default used"

    if alg == 0:
        algorithm = "unknown"
    elif alg == 1 or alg == 2:
        algorithm = "trackChi2"
    elif alg == 3:
        algorithm = "vertexLL"
    elif alg == 4:
        algorithm = "vertexChi2"
    else:
        algorithm = "unknown"

    return (success, online, fitWidth, algorithm )


from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)

(options,args) = parser.parse_args()
if len(args) < 1:
    parser.error('wrong number of command line arguments')

for i in args:
    print getStatus(i)

