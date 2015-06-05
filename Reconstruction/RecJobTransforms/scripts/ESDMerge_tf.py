#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## ESDMerge_tf.py - ESD merger
# @version $Id: ESDMerge_tf.py 670203 2015-05-27 13:19:01Z graemes $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, hybridPOOLMergeExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

ListOfDefaultPositionalKeys=['--DBRelease', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--fastPoolMerge', '--geometryVersion', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputESDFile', '--maxEvents', '--noimf', '--notcmalloc', '--outputESD_MRGFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation']

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
    executorSet.add(hybridPOOLMergeExecutor(name = 'ESDMerge', skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                                   inData = ['ESD'], outData = ['ESD_MRG']))

    trf = transform(executor = executorSet)
    
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('ESDMerge_tf', 'ESD merge job specific options')
    parser.add_argument('--inputESDFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input ESD file', group='ESDMerge_tf')
    parser.add_argument('--outputESD_MRGFile', '--outputESDFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output merged ESD file', group='ESDMerge_tf')
    parser.add_argument('--fastPoolMerge', type=trfArgClasses.argFactory(trfArgClasses.argBool),
                        help='Hybrid POOL merging switch')


if __name__ == '__main__':
    main()
