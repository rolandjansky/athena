#!/usr/bin/env python

## FTK Simulation Transform
# @version $Id: FTKConstantGen_tf.py 718878 2016-01-20 17:27:51Z jwebster $ 
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
    trf = transform(executor = athenaExecutor(name = 'FTKConstantGen', 
                                              skeletonFile = 'TrigFTKBankGen/skeleton.FTKConstantGen.py'))
    addFTKPattGenArgs(trf.parser)
    return trf


def addFTKPattGenArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('FTKConstantGen', 'Fast tracker simulation options')

    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of pattern banks', group='FTKConstantGen')

    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Bank region number', group='FTKConstantGen')
   
    # parser.add_argument('--allregions', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
    #                     help='Merge all regions', group='FTKConstantGen')
    
    parser.add_argument('--genconst', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Generate the sectors and constants', group='FTKConstantGen')
    
    parser.add_argument('--inputFTKMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True), 
                       help="input matrix file", group="FTKConstantGen" )

    parser.add_argument('--ITkMode', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Use ITk geometry', group='FTKConstantGen')

if __name__ == '__main__':
    main()
