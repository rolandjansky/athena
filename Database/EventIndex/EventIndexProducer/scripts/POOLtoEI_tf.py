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
msg.info('logging set in {}'.format(sys.argv[0]))

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is {}'.format(sys.argv[0]))
    
    trf = getTransform()    
    trf.parseCmdLineArgs(sys.argv[1:])

    # update datasetname if inputfile has the form "datasetname#filename"
    # and it has not been specified within the options.
    args = trf.argdict
    if not args.has_key("eidsname"):
        inputPOOLFile = args['inputPOOLFile']
        dsname = inputPOOLFile.dataset
        if dsname is not None:
            obj=trfArgClasses.argString(dsname)
            args['eidsname'] = obj


    trf.execute()
    trf.generateReport()

    msg.info("{} stopped at {}, trf exit code {:d}".format(sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

## Get the base transform with all arguments added
def getTransform():
    trf = transform(executor = athenaExecutor(name = 'POOLtoEI', 
                                              skeletonFile = 'EventIndexProducer/skeleton.POOLtoEI_tf.py'))
    addAthenaArguments(trf.parser)
    addMyArgs(trf.parser)
    return trf
    
    
def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Event Index', 'Options for event index generation')
    parser.add_argument('--inputPOOLFile', required=True,  ## nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input POOL file', group='Event Index')
    parser.add_argument('--outputEIFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='output', type='misc'),
                        help='Output Event Index file (default: pool.ei.pkl)', group='Event Index')
    parser.add_argument("--eidsname", 
                        type=trfArgClasses.argFactory(trfArgClasses.argString),
                        help="Overrides input file dataset name (default: read from job environment variable INDS)", 
                        group='Event Index')
    parser.add_argument("--trigger", 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool),
                        help="Include trigger information (default: true)", group='Event Index')
    parser.add_argument("--provenance", 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool),
                        help="Include provenance information (default: true)", group='Event Index')
    parser.add_argument("--sendtobroker", 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool),
                        help="Send event index to message broker (default: true)", group='Event Index')

    # internal options for T0 jobs
    parser.add_argument("--_taskid", 
                        type=trfArgClasses.argFactory(trfArgClasses.argString),
                        help="TaskID (for T0 jobs usage)", group='Event Index')
    parser.add_argument("--_jobid", 
                        type=trfArgClasses.argFactory(trfArgClasses.argString),
                        help="JobID (for T0 jobs usage)", group='Event Index')
    parser.add_argument("--_attempt", 
                        type=trfArgClasses.argFactory(trfArgClasses.argString),
                        help="Attempt number (for T0 jobs usage)", group='Event Index')

if __name__ == '__main__':
    main()
