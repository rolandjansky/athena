#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## TAG file merger, adapted from Tier0 TAGMerge_trf.py
#  Uses CollAppend binary
# @version $Id: TAGMerge_tf.py 571113 2013-11-18 21:45:05Z graemes $ 

import os.path
import sys
import time

import logging

# This is a hack for Prodsys I. To generate this list use the --dumpargs option to the transform, then paste in here
# N.B. This all has to go on one line!
ListOfDefaultPositionalKeys=['--amiConfig', '--amiMetadataTag', '--checkEventCount', '--env', '--execOnly', '--ignoreErrors', '--inputTAGFile', '--noimf', '--notcmalloc', '--outputTAG_MRGFile', '--reportName', '--showGraph', '--showPath', '--showSteps', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation']

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import tagMergeExecutor
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
    trf = transform(executor = tagMergeExecutor(name = 'TAGFileMerge', exe = 'CollAppend', 
                                                inData = set(['TAG']), outData = set(['TAG_MRG'])))
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('TAGMerge_tf', 'TAG merging specific options')
    parser.add_argument('--inputTAGFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='input'),
                        help='Input TAG file(s)', group='TAGMerge_tf')
    parser.add_argument('--outputTAG_MRGFile', '--outputTAGFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='output'),
                        help='Output merged TAG file', group='TAGMerge_tf')


if __name__ == '__main__':
    main()
