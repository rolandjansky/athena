#!/usr/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

'''
@file HTTLogicalHitsToAlg_tf.py
@date July 14, 2020
@author Riley Xu - riley.xu@cern.ch
@brief This transform runs HTTLogicalHitsProcessAlgo.

Usage:
    HTTLogicalHitsToAlg_tf.py \
            --maxEvents 100 \
            --InFileName "hits.raw.root" \
            --OutFileName "hits.logical.root" \
'''

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

from TrigHTTConfTools.parseRunArgs import addHTTMapsArgs, addHTTBanksArgs, addHTTAlgorithmsArgs, addHTTHitFilteringArgs

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
    trf = transform(executor = athenaExecutor(name = 'HTTLogicalHitsToAlg',
                                              skeletonFile = 'TrigHTTAlgorithms/skeleton.HTTLogicalHitsToAlg.py'))
    addAthenaArguments(trf.parser)
    addHTTMapsArgs(trf.parser)
    addHTTBanksArgs(trf.parser)
    addHTTAlgorithmsArgs(trf.parser)
    addHTTLogicalHitsToAlgArgs(trf.parser)
    addHTTHitFilteringArgs(trf.parser)
    return trf



# ___________________________________________________________________________ #
def addHTTLogicalHitsToAlgArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTLogicalHitsToAlg', 'Options for HTTLogicalHitsToAlg')

    # Enable easy copy-paste from C++ argument initializers
    def declareProperty(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTLogicalHitsToAlg')

    declareProperty("InFileName", trfArgClasses.argList, "input raw hit file path");
    declareProperty("InFileName2", trfArgClasses.argString, "input raw hit file path for 2nd input tool");
    declareProperty("OutFileName", trfArgClasses.argString, "output logical hit file path");


if __name__ == '__main__':
    main()
