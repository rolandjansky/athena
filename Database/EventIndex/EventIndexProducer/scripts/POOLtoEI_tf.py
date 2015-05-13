#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Run EventIndex on POOL file, produce EI file and send Events to brokers
"""

import argparse
import os.path
import sys
import time
import traceback

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in {}'.format(sys.argv[0]))

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

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
        dsname=None
        for filetype in ('POOL', 'AOD', 'ESD', 'EVNT', 'HITS', 'RDO'):
            if args.has_key('input'+filetype+'File'):
                inputFile = args['input'+filetype+'File']
                dsname = inputFile.dataset
        if dsname is not None:
            obj=trfArgClasses.argString(dsname)
            args['eidsname'] = obj


    trf.execute()
    trf.generateReport()

    msg.info("{} stopped at {}, trf exit code {:d}".format(sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

## Get the base transform with all arguments added
def getTransform():
    executorSet = set()
    from EventIndexProducer.EITransformUtils import addEI_Substep,  addEI_arguments
    addEI_Substep(executorSet)

    trf = transform(executor = executorSet, description = 'EventIndex transform. Input must be a POOL file.')
    addAthenaArguments(trf.parser)
    addEI_arguments(trf.parser)
    return trf
    
    
if __name__ == '__main__':
    main()
