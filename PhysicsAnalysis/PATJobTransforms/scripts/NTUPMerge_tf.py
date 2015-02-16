#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## MergeNTUP_tf.py - NTUPLE merger
# @version $Id: NTUPMerge_tf.py 647214 2015-02-16 17:16:20Z graemes $

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, NTUPMergeExecutor
from PyJobTransforms.trfArgs import addD3PDArguments, addExtraDPDTypes
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from PATJobTransforms.PATTransformUtils import addNTUPMergeSubsteps, addPhysValidationMergeFiles
import PyJobTransforms.trfArgClasses as trfArgClasses


@stdTrfExceptionHandler
@sigUsrStackTrace

def main():
    
    msg.info('This is %s' % sys.argv[0])
    if sys.argv[1:] == []:
        msg.info("%s stopped at %s, no input parameters given" % (sys.argv[0], time.asctime()))
    
    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()
    msg.info("%s stopped at %s, tf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


def getTransform():
    executorSet = set()
    addNTUPMergeSubsteps(executorSet)
    trf = transform(executor = executorSet)
    addPhysValidationMergeFiles(trf.parser)
    addD3PDArguments(trf.parser, transform=trf, addD3PDMRGtypes=True)
    addExtraDPDTypes(trf.parser, transform=trf, NTUPMergerArgs = True)
    return trf

if __name__ == '__main__':
    main()

