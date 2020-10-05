#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## A simple executor wrapper around the transforms' scriptExecutor
#  Mainly used to test core infrastructure
# $Id: ExeWrap_tf.py 634752 2014-12-09 15:01:52Z graemes $

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
    
    # Need to update what we want to execute after the command line is parsed
    # LHS is the slightly convoluted way to get at the single member of a set 
    # (which, of course, itself has no index)
    list(trf._executors)[0].exe = trf.argdict['exe'].value
    
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d", (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():
    trf = transform(executor = scriptExecutor(name = 'ExeWrap', exe = None, exeArgs = ['args']))
    addMyArgs(trf.parser)
    return trf

## Example of how to add some specific arguments to the transform 
def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('ExeWrap_trf', 'ExeWrap_trf specific options')
    parser.add_argument('--exe', type=trfArgClasses.argFactory(trfArgClasses.argString), 
                          group='ExeWrap_trf', help='Executable to invoke')
    parser.add_argument('--args', type=trfArgClasses.argFactory(trfArgClasses.argList), nargs='+',
                          group='ExeWrap_trf', help='Additonal parameters to the executor')


if __name__ == '__main__':
    main()
