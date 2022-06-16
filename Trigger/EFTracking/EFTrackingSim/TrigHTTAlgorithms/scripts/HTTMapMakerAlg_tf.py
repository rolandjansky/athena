# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python


import argparse
import sys
import time
import traceback
import logging

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigHTTConfTools.parseRunArgs import addHTTAlgorithmsArgs

@stdTrfExceptionHandler
@sigUsrStackTrace

# ___________________________________________________________________________ #
def main():

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


# ___________________________________________________________________________ #
def getTransform():
    # Get the base transform with all arguments added
    trf = transform(executor = athenaExecutor(name = 'HTTMapMakerAlg',
                                              skeletonFile = 'TrigHTTAlgorithms/skeleton.HTTMapMakerAlg.py'))
    addAthenaArguments(trf.parser)
    addHTTAlgorithmsArgs(trf.parser)
    addHTTMapMakerAlgArgs(trf.parser)

    return trf



# ___________________________________________________________________________ #
def addHTTMapMakerAlgArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTMapMakerAlg', 'Options for HTTMapMakerAlg')

    # Enable easy copy-paste from C++ argument initializers
    def declareProperty(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTMapMakerAlg')

    declareProperty("InFileName", trfArgClasses.argList, "input raw hit file path")
    declareProperty("OutFileName", trfArgClasses.argString, "output map file path")
    declareProperty("region", trfArgClasses.argInt, "region")
    declareProperty("KeyString", trfArgClasses.argString, "key layer for subrmap, i.e. 'strip,barrel,2' or 'plane 4'")
    declareProperty("KeyString2", trfArgClasses.argString, "second key layer used for 2D slicing")
    declareProperty("nSlices", trfArgClasses.argInt, "number of slices to divide key layer into, default is full granularity")
    declareProperty("trim", trfArgClasses.argFloat, "trim modules with less than given percent of tracks")
    declareProperty("maxEvents", trfArgClasses.argInt, "Number of input events to process")


if __name__ == '__main__':
    main()
