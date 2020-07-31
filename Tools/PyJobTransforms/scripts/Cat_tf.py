#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## A simple 'cat' transform which just cats some input files
# $Id: Cat_tf.py 529035 2012-12-05 15:45:24Z graemes $

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import scriptExecutor
import PyJobTransforms.trfArgClasses as trfArgClasses
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

def getTransform():
    trf = transform(executor = scriptExecutor(name = 'The Fabulous Cat', exe = 'cat', exeArgs = ['inFile']))
    addMyArgs(trf.parser)
    return trf

## Example of how to add some specific arguments to the transform 
def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Cat_trf', 'Cat_trf specific options')
    parser.add_argument('--inFile', type=trfArgClasses.argFactory(trfArgClasses.argString), 
                          group='Cat_trf', help='Input file for the cat')


if __name__ == '__main__':
    main()
