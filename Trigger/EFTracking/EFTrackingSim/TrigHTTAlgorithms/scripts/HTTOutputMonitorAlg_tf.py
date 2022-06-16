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

from TrigHTTConfTools.parseRunArgs import addHTTMapsArgs, addHTTBanksArgs, addHTTAlgorithmsArgs

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
    trf = transform(executor = athenaExecutor(name = 'HTTOutputMonitorAlg',
                                              skeletonFile = 'TrigHTTAlgorithms/skeleton.HTTOutputMonitorAlg.py'))
    addAthenaArguments(trf.parser)
    addHTTMapsArgs(trf.parser)
    addHTTBanksArgs(trf.parser)
    addHTTAlgorithmsArgs(trf.parser)
    addHTTOutputMonitorAlgArgs(trf.parser)
    return trf



# ___________________________________________________________________________ #
def addHTTOutputMonitorAlgArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTOutputMonitorAlg', 'Options for HTTOutputMonitorAlg')

    # Enable easy copy-paste from C++ argument initializers
    def declareProperty(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTOutputMonitorAlg')

    declareProperty("InFileName", trfArgClasses.argString, "output logical hit file path");
    declareProperty("OutFileName", trfArgClasses.argString, "output monitoring histogram ROOT file path");
    declareProperty("OutputLevel", trfArgClasses.argInt, "set OutputLevel: DEBUG=2, INFO=3");


if __name__ == '__main__':
    main()
