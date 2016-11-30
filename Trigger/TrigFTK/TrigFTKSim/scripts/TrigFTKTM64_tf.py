#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Transform
#  Specialist version to do sim x 4 subregions and merge in one job
# @version $Id: TrigFTKSM4Un_tf.py 579246 2014-01-20 17:48:57Z gvolpi $

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

from TrigFTKSim.FTKSimOptions import *

ntowers = 64
subregions = 1


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

    executorSet = set()
    for tower in range(ntowers) :
        executorSet.add(athenaExecutor(name = 'FTKFullSimulationBank{0}'.format(tower),
              skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneSim.py',disableMP=True,
              inData = ['NTUP_FTKIP','TXT_FTKIP'], outData = ['NTUP_FTKTMP_{0}'.format(tower)],
              extraRunargs = {'bankregion': [tower],'banksubregion': [0]},
              # Need to ensure that the correct subregion is used
              runtimeRunargs = {'bankregion': [tower], 'subregions': [0],
               'patternbankpath': '[runArgs.patternbankspath[{0}]]'.format(tower),
               'fitconstantspath': '[runArgs.fitconstantspath[{0}]]'.format(tower),
               'fit711constantspath': '[runArgs.fit711constantspath[{0}]]'.format(tower),
               'sectorpath': '[runArgs.sectorspath[{0}]]'.format(tower),
               'outputNTUP_FTKTMPFile': 'runArgs.outputNTUP_FTKTMP_{0}File'.format(tower)}))

    # add final merge for all the tower, generating a NTUP_FTK file
    executorSet.add(athenaExecutor(name = "FTKSimulationMergeFinal",
          skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneMerge.py',disableMP=True,
          inData = [tuple([ 'NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers) ])+('NTUP_FTKIP',)],
          outData = ['NTUP_FTK'],
          extraRunargs = {'inputNTUP_FTKTMPFile': [ 'tmp.NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers)]},
          runtimeRunargs = {'MergeFromTowers': False,
                            'FirstRegion': 0,
                            'TruthTrackTreeName' : "'truthtracks'",
                            'EvtInfoTreeName' : "'evtinfo'",
                            'SaveTruthTree' : '1'}))

    trf = transform(executor = executorSet, description = 'FTK simulation for {0} towers, with no subregions, and final merge.'.format(ntowers))

    addFTKSimulationArgs(trf.parser)
    addTrigFTKSimMergeOptions(trf.parser);
    addTrigFTKSimOptions(trf.parser,nsubregions=subregions)
    addTrigFTKSimRFOptions(trf.parser)
    addTrigFTKSimTFOptions(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')

    # File handling
    parser.add_argument('--inputNTUP_FTKIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkip', treeNames='ftkhits'),
                        help='FTK RDO file in ROOT format', group='TrigFTKSim', nargs='+')
    parser.add_argument('--inputTXT_FTKIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, runarg=True, io='input', type='txt_ftkip'),
                        help='Wrapper files (in .dat.bz2 format)', group='TrigFTKSim', nargs='+')

    # The following for testing only
    for tower in range(ntowers) :
        parser.add_argument('--outputNTUP_FTKTMP_{0}File'.format(tower),
                            type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                            help='Tower {0} merged FTK file (for test only)'.format(tower), group='TrigFTKMerge')
        parser.add_argument('--inputNTUP_FTKTMP_{0}File'.format(tower),
                            type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftk', treeNames='ftkdata'),
                            help='FTK NTUP file from tower {0}  (for testing only)'.format(tower), group='TrigFTKMerge')

    parser.add_argument('--outputNTUP_FTKFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='FTK merged tracks file', group='TrigFTKMerge')
    parser.add_argument('--FTKUnmergedInputPath',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')

    parser.add_argument('--FTKForceAllInput',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')

    parser.add_argument('--FTKDoGrid',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')


if __name__ == '__main__':
    main()
