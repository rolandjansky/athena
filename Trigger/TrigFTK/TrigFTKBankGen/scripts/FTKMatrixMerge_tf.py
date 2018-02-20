#!/usr/bin/env python

## FTK Simulation Transform
# @version $Id: FTKMatrixMerge_tf.py 574395 2013-12-07 10:13:16Z gvolpi $ 
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
    trf = transform(executor = athenaExecutor(name = 'FTKMatrixMerge', 
                                              skeletonFile = 'TrigFTKBankGen/skeleton.FTKMatrixMerge.py'))
    addFTKPattGenArgs(trf.parser)
    return trf


def addFTKPattGenArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('FTKMatrixMerge', 'Fast tracker simulation options')

    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of pattern banks', group='FTKMatrixMerge')

    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Bank region number', group='FTKMatrixMerge')
   
    parser.add_argument('--allregions', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Merge all regions', group='FTKMatrixMerge')
    
    parser.add_argument('--genconst', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Generate the sectors and constants', group='FTKMatrixMerge')
    
    parser.add_argument('--inputFTKMatrixFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input'),

                        help='List of matrix files',group='FTKMatrixMerge',nargs='+')

    parser.add_argument('--FTKMatrixFileRegEx', group="FTKMatrixMerge",
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help="Regular expression to select the input files from the working dir")

    parser.add_argument('--MaxInputFiles', group="FTKMatrixMerge",
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Max number of input files to include, only interpretted when used in combination with FTKMatrixFileRegEx")

    parser.add_argument('--outputMergedFTKMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True),
                       help="Output file", group="FTKMatrixMerge" )

    parser.add_argument('--ITkMode', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Use ITk geometry', group='FTKMatrixMerge')

if __name__ == '__main__':
    main()
