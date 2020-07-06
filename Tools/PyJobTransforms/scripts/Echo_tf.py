#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## A simple 'echo' transform which merely prints its arguments and exits
# $Id: Echo_tf.py 532364 2013-01-09 15:51:55Z graemes $

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import echoExecutor
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgs as trfArgs
import PyJobTransforms.trfArgClasses as trfArgClasses

# Always embed your transform inside a top level exception
# handler. This ensures that uncaught exceptions are handled
# with a modicum of decency and that the transform has the 
# chance to produce a job report and a sensible exit code.
# These decorators do this.
@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s', sys.argv[0])
        
    trf = getTransform()

    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, transform exit code %d", sys.argv[0], time.asctime(), trf.exitCode)
    sys.exit(trf.exitCode)

def getTransform():
    trf = transform(executor = echoExecutor())
    addMyArgs(trf.parser)
    trfArgs.addTeaArguments(trf.parser)
    trfArgs.addAthenaArguments(trf.parser)
    return trf


## Example of how to add some specific arguments to the transform 
def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Echo_trf', 'Echo_trf specific options')
    parser.add_argument('--testInt', group='Echo_trf', type=trfArgClasses.argFactory(trfArgClasses.argInt), help='An integer')
    parser.add_argument('--testFloat', group='Echo_trf', type=trfArgClasses.argFactory(trfArgClasses.argFloat), help='A float')
    parser.add_argument('--testList', group='Echo_trf', 
                        type=trfArgClasses.argFactory(trfArgClasses.argList), help='A string list', nargs='+')
    parser.add_argument('--testIntList', group='Echo_trf', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argIntList), help='A int list')
    parser.add_argument('--testFile', group='Echo_trf', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='input'), help='Test file(s)')
    parser.add_argument('--testSubstepList', group='Echo_trf', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepList), help='A substep list')
    parser.add_argument('--testSubstepInt', group='Echo_trf', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt), help='A substep int')
    parser.add_argument('--testSubstepBool', group='Echo_trf', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepBool), help='A substep bool')
    


if __name__ == '__main__':
    main()
