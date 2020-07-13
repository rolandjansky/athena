#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## A simple 'sleep' transform which just sleeps
# Useful for testing behaviour with signals and stuff
# $Id: Sleep_tf.py 534178 2013-01-21 19:04:08Z graemes $

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

    msg.info("%s stopped at %s, trf exit code %d", (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


def getTransform():
    trf = transform(executor = scriptExecutor(name = 'The Sandman', exe = 'sleep', exeArgs = ['dust']))
    addMyArgs(trf.parser)
    return trf

## Example of how to add some specific arguments to the transform 
def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Sleep_trf', 'Sleep_trf specific options')
    parser.add_argument('--dust', type=trfArgClasses.argFactory(trfArgClasses.argInt), group='Sleep_trf',
                          help='How much dust to throw (=sleep time in seconds)', default=trfArgClasses.argInt(10))


if __name__ == '__main__':
    main()
