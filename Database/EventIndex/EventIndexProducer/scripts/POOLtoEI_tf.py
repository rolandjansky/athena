#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import argparse
import os.path
import sys
import time
import traceback

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
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

## Get the base transform with all arguments added
def getTransform():
    trf = transform(executor = athenaExecutor(name = 'POOLtoEI', 
                                              skeletonFile = 'EventIndexProducer/skeleton.POOLtoEI_tf.py'))
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addMyArgs(trf.parser)
    return trf
    
    
def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('POOLtoEI_tf', 'POOL to EI specific options')
    parser.add_argument('--inputPOOLFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input POOL file', group='POOLtoEI_tf')
    parser.add_argument('--outputEIFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='output', type='misc'),
                        help='Output EI file', group='POOLtoEI_tf')
    parser.add_argument("--trigger", 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Include trigger information (default: false)")
    parser.add_argument("--provenance", 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Include provenance information (default: true)")

if __name__ == '__main__':
    main()
