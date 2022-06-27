# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python

## HTT Simulation Transform
# @version $Id: HTTConstantGen_tf.py 718878 2016-01-20 17:27:51Z jwebster $
import argparse
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
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses
from TrigHTTConfig.parseRunArgs import addHTTMapsArgs

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
    trf = transform(executor = athenaExecutor(name = 'HTTConstantGen',
                                              skeletonFile = 'TrigHTTBankGen/skeleton.HTTConstantGen.py'))
    addHTTPattGenArgs(trf.parser)
    addHTTMapsArgs(trf.parser)
    return trf


def addHTTPattGenArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTConstantGen', 'Fast tracker simulation options')

    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of pattern banks', group='HTTConstantGen')

    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Bank region number', group='HTTConstantGen')

    parser.add_argument('--genconst', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Generate the sectors and constants', group='HTTConstantGen')

    parser.add_argument('--skip_sectors', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Sectors to skip', group='HTTConstantGen')

    parser.add_argument('--inputHTTMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True),
                       help="input matrix file", group="HTTConstantGen" )

    parser.add_argument('--outputHTTGoodMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='output'), help="output good matrix file", group="HTTConstantGen")


    parser.add_argument('--outputHTTGoodMatrixReducedCheckFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='output',type='htt_matrix_good_reduced_check'), help="output reduced good matrix file, check whether 2nd stage is good or not", group="HTTConstantGen")


if __name__ == '__main__':
    main()
