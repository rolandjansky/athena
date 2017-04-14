#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Transform
#  Specialist version to do sim x 4 subregions and merge in one job
# @version $Id: TrigFTKSM4Un_tf.py 725454 2016-02-22 09:44:33Z sschmitt $

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

subregions = 1

from TrigFTKSim.FTKSimOptions import *


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():

    print '\ntesting if this is the correct transform...\n\n'


    msg.info('This is %s' % sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


## Get the base transform with all arguments added
def getTransform():

    executorSet = set()
    for subregion in range(subregions):
        executorSet.add(athenaExecutor(name = 'FTKFullSimulationBank{0}'.format(subregion),
                                       skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneSim.py',
                                       inData = ['NTUP_FTKIP','TXT_FTKIP'], outData = ['NTUP_FTKTMP'],
                                       extraRunargs = {'banksubregion': [subregion]},
                                       # Need to ensure that the correct subregion is used
                                       runtimeRunargs = {'patternbankpath': 'runArgs.patternbank{0}path'.format(subregion),
                                                         'outputNTUP_FTKTMPFile': 'runArgs.outputNTUP_FTKFile',
                                                         'cachedbankpath': 'runArgs.cachedbank{0}path'.format(subregion),
                                                         'CachePath': 'runArgs.CachePath{0}'.format(subregion)   }))

    trf = transform(executor = executorSet, description = 'FTK Subregion simulate x {0} .'.format(subregions))

    addFTKSimulationArgs(trf.parser)
    addTrigFTKSimOptions(trf.parser,nsubregions=subregions)
    addTrigFTKSimMergeOptions(trf.parser);
    addTrigFTKSimTFOptions(trf.parser)
    addTrigFTKSimRFOptions(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation options')

    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                            help='Bank region number', group='TrigFTKSim', nargs='+')
    parser.add_argument('--sectorpath', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                           help='sectors path file for all the subregions', group='TrigFTKSim', nargs='+')

    # Add named parameters for each subregion
    for subregion in range(subregions):
        parser.add_argument('--patternbank{0}path'.format(subregion), type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                            help='Pattern bank path file, subregion {0}'.format(subregion), group='TrigFTKSim', nargs='+')

        parser.add_argument('--cachedbank{0}path'.format(subregion), type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                            help='Output cached bank path file, subregion {0}'.format(subregion), group='TrigFTKSim')

        parser.add_argument('--CachePath{0}'.format(subregion), type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                            help='Output cached bank path file, subregion {0}'.format(subregion), group='TrigFTKSim')


    parser.add_argument('--ssmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='superStrip file for AM patterns',  group='TrigFTKSim')

    parser.add_argument('--SetAMSplit', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='set splitting algo; 0 (default) means no-splitting', group='TrigFTKSim')


    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')

    # File handling
    parser.add_argument('--inputNTUP_FTKIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptmp', treeNames='ftkhits'),
                        help='FTK RDO file in ROOT format'.format(subregion), group='TrigFTKMerge', nargs='+')
    parser.add_argument('--inputTXT_FTKIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, runarg=True, io='input', type='txt_ftkip'),
                        help='Wrapper files (in .dat.bz2 format)', group='TrigFTKSim', nargs='+')
    parser.add_argument('--outputNTUP_FTKTMPFile', '--outputNTUP_FTKFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='Subregion merged FTK file'.format(subregion), group='TrigFTKMerge',nargs='+')
    parser.add_argument('--HWModeSS', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='SSID encoding scheme', group='TrigFTKSim')
    parser.add_argument('--ModuleLUTPath', default=0, type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='module LUT for HWModeSS=2', group='TrigFTKSim')
    parser.add_argument('--ModuleLUTPath2nd', default=0, type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='module LUT for HWModeSS=2, unused layers', group='TrigFTKSim')
    parser.add_argument('--MaxMissingSCTPairs', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='6/8 option for transition region', group='TrigFTKSim')
    parser.add_argument('--UseTSPBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='TSP bank utilisation', group='TrigFTKSim')
    parser.add_argument('--UseCompressedBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='use compressed bank algorithm', group='TrigFTKSim')
    parser.add_argument('--badmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='bad module map', group='TrigFTKSim')


    # The following for testing only
    for subregion in range(subregions):
        parser.add_argument('--inputNTUP_FTKTMP_{0}File'.format(subregion),
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptmp', treeNames='ftkdata'),
                        help='FTK NTUP file from subregion {0} (for testing only)'.format(subregion), group='TrigFTKSim', nargs='+')
        parser.add_argument('--outputNTUP_FTKTMP_{0}File'.format(subregion),
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftkiptmp', treeNames='ftkdata'),
                        help='FTK NTUP file from subregion {0} (for testing only)'.format(subregion), group='TrigFTKSim', nargs='+')




if __name__ == '__main__':
    main()
