#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Merge Transform
# @version $Id: TrigFTKMerge_tf.py 519203 2012-09-27 12:02:09Z gvolpi $

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
from PyJobTransforms.trfArgs import addAthenaArguments
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
    trf = transform(executor = athenaExecutor(name = 'FTKSimulationMerge',disableMP=True,
                                              skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneMerge.py'))
    addAthenaArguments(trf.parser)
    addFTKMergerArgs(trf.parser)
    return trf


def addFTKMergerArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation options')
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')

    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of pattern banks', group='TrigFTKSim')
    parser.add_argument('--NSubRegions', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of sub-regions', group='TrigFTKSim')
    parser.add_argument('--FirstRegion', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='First ID of the region (def: 0)',group='TrigFTKMerge')
    parser.add_argument('--FirstSubreg', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='First ID of the subregions (def: 0)',group="TrigFTKMerge")

    parser.add_argument('--pmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of pmap file', group='TrigFTKMerge')
    parser.add_argument('--pmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of pmapunused file', group='TrigFTKMerge')

    parser.add_argument('--loadHWConf_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),

                        help='Location of HW configuration file', group='TrigFTKMerge')
    parser.add_argument('--inputNTUP_FTKTMPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftktmp', treeNames='ftkdata'),
                        help='Input file path', group='TrigFTKMerge',nargs='+')
    parser.add_argument('--outputNTUP_FTKFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='Output file path', group='TrigFTKMerge',nargs='+')

    parser.add_argument('--FTKSetupTag', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Setup the FTK architecture tag, if specific values are also used they have priority', group='TrigFTKMerge')
    parser.add_argument('--FTKUnmergedInputPath',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')

    parser.add_argument('--FTKForceAllInput',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')

    parser.add_argument('--FTKDoGrid',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')

    parser.add_argument('--MergeRegion',
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="In case of merge for a single region this optionsets which region is merged", group="TrigFTKMerge")

    parser.add_argument('--MergeRoads',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Merge roads", group="TrigFTKMerge")

    parser.add_argument('--MergeFromTowers',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="This option can be used to merge NTUP_FTK files produced from a previous partial merging job", group="TrigFTKMerge")
    parser.add_argument('--SaveTruthTree',
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Add truth information to output", group="TrigFTKMerge")
    parser.add_argument('--inputNTUP_FTKIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptruth'),
                        help="Original FTKIPO file with truth information", group="TrigFTKMerge", nargs='+')
    parser.add_argument('--TruthTrackTreeName',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Truth tracks tree name', group='TrigFTKMerge')
    parser.add_argument('--EvtInfoTreeName',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Event info tree name', group='TrigFTKMerge')

if __name__ == '__main__':
    main()
