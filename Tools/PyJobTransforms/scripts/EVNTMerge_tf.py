#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## AODMerge_tf.py - AOD merger with optional TAG and DPD outputs
#  N.B. Do need clarification as to if AODtoDPD is ever run in parallel with AOD merging 
# @version $Id: AODMerge_tf.py 530225 2012-12-12 18:16:17Z graemes $ 

from __future__ import print_function
import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s', sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d", sys.argv[0], time.asctime(), trf.exitCode)
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
    print(type(executorSet))
    executorSet.add(athenaExecutor(name = 'EVNTMerge', skeletonFile = 'PyJobTransforms/skeleton.EVNTMerge.py',inData = ['EVNT'], outData = ['EVNT_MRG']))

    trf = transform(executor = executorSet)
    
    addAthenaArguments(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('EVNTMerge_tf', 'EVNT merge job specific options')
    parser.add_argument('--inputEVNTFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='input', runarg=True, type='evnt'),
                        help='Input EVNT file', group='EVNTMerge_tf')
    parser.add_argument('--outputEVNT_MRGFile', '--outputEVNTFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='output', runarg=True, type='evnt'),
                        help='Output merged EVNT file', group='EVNTMerge_tf')
    parser.add_argument('--eventService', type=trfArgClasses.argFactory(trfArgClasses.argBool), metavar = "BOOL",
                        help='Switch AthenaMP to the Event Service configuration', group='EVNTMerge_tf')
    
    


if __name__ == '__main__':
    main()
