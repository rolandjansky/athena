#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#! /usr/bin/env python

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s' % sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
    print type(executorSet)
    executorSet.add(athenaExecutor(name = 'EvtGenFromEVNT', skeletonFile = 'EvgenJobTransforms/skeleton.EvtGenFromEVNT.py',inData = ['EVNT'], outData = ['EVNT_MRG']))

    trf = transform(executor = executorSet)
    
    addAthenaArguments(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('EvtGenFromEVNT_tf', 'EVNT merge job specific options')
    parser.add_argument('--inputEVNTFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='evnt'),
                        help='Input EVNT file', group='EvtGenFromEVNT_tf')
    parser.add_argument('--outputEVNT_MRGFile', '--outputEVNTFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', runarg=True, type='evnt'),
                        help='Output EVNT file', group='EvtGenFromEVNT_tf')
    
    


if __name__ == '__main__':
    main()
