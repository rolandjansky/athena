#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## A simple tranform running HelloWorld 

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

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
    trf = transform(executor = athenaExecutor(name = 'athena', 
                                              substep='HelloWorld',
                                              skeletonFile=None,
                                              skeletonCA="PyJobTransforms.HelloWorldSkeleton"))
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    return trf
    
if __name__ == '__main__':
    main()
