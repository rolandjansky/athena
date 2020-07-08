#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## A simple athena transform. 
# @version $Id: Athena_tf.py 557865 2013-08-12 21:54:36Z graemes $ 

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
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

## Get the base transform with all arguments added
def getTransform():
    trf = transform(executor = athenaExecutor(name = 'athena'))
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addMyArgs(trf.parser)
    return trf
    
    
def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Athena_trf', 'Athena_trf specific options')
    parser.add_argument('--testInt', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                          help='Test integer argument', group='Athena_trf')
    parser.add_argument('--testFloat', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                          help='Test float argument', group='Athena_trf')
    parser.add_argument('--maxMsgLevel', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                          help='Highest message level to print in athena (not yet implemented!)', group='Athena_trf')


if __name__ == '__main__':
    main()
