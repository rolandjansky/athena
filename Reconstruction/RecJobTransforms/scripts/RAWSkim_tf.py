#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## SkimRAW_tf.py - Bytestream skimmer transform
# @version $Id: RAWSkim_tf.py 643395 2015-02-01 19:51:21Z graemes $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

from RecJobTransforms.recoTransforms import skimRawExecutor

ListOfDefaultPositionalKeys=['--amiConfig', '--amiMetadataTag', '--checkEventCount', '--env', '--execOnly', '--filterFile', '--ignoreErrors', '--inputBSFile', '--noimf', '--notcmalloc', '--outputBS_SKIMFile', '--reportName', '--showGraph', '--showPath', '--showSteps', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation']

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
    executorSet.add(skimRawExecutor(name = 'SkimRAW', inData = ['BS'], outData = ['BS_SKIM'], exe = 'acmd.py'))

    trf = transform(executor = executorSet)
    
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('SkimRAW', 'Skim RAW Options')
    parser.add_argument('--inputBSFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input', type='bs'),
                        help='Input bytestream file(s)', group='SkimRAW', metavar='FILE')
    parser.add_argument('--outputBS_SKIMFile', '--outputBSFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output', type='bs'),
                        help='Output skimmed bytestream file', group='SkimRAW', metavar='SkimmedFILE')
    parser.add_argument('--filterFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argString),
                        help='Filter file for skimming bytestream', group='SkimRAW')


if __name__ == '__main__':
    main()
