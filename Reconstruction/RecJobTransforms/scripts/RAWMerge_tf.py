#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Bytestream file merger, adapted from Tier0 RAWMerge_trf.py
#  Uses TDAQ fast bytestream merger 'file_merging'
# @version $Id: RAWMerge_tf.py 571113 2013-11-18 21:45:05Z graemes $ 

import os.path
import sys
import time

import logging

# This is a hack for Prodsys I. To generate this list use the --dumpargs option to the transform, then paste in here
# N.B. This all has to go on one line!
ListOfDefaultPositionalKeys=['--allowRename', '--amiConfig', '--amiMetadataTag', '--checkEventCount', '--env', '--execOnly', '--ignoreErrors', '--inputBSFile', '--maskEmptyInputs', '--noimf', '--notcmalloc', '--outputBS_MRGFile', '--reportName', '--showGraph', '--showPath', '--showSteps', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation']

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import bsMergeExecutor
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
    trf = transform(executor = bsMergeExecutor(name = 'RAWFileMerge', exe = 'file_merging',
                                               inData = set(['BS']), outData = set(['BS_MRG'])))

    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('RAWMerge_tf', 'RAWMerge specific options')
    parser.add_argument('--inputBSFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                        help='Input BS file(s)', group='RAWMerge_tf')
    parser.add_argument('--outputBS_MRGFile', '--outputBSFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output'),
                        help='Output merged BS file (best if this file ends in ._0001.data, but see allowRename option below)', 
                        group='RAWMerge_tf')
    parser.add_argument('--maskEmptyInputs', type=trfArgClasses.argFactory(trfArgClasses.argBool), group='RAWMerge_tf',
                        help='If true then empty BS files are not included in the merge (default True)', 
                        default=trfArgClasses.argBool('True'))
    parser.add_argument('--allowRename', type=trfArgClasses.argFactory(trfArgClasses.argBool), group='RAWMerge_tf',
                        help='If true merged BS file will be forcibly renamed to the value of "outputBSFile" (default True)', 
                        default=trfArgClasses.argBool('True'))


if __name__ == '__main__':
    main()
