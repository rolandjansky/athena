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
from PyJobTransforms.trfArgs import addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

thistower = 22
subregions = 1

from TrigFTKSim.FTKSimOptions import *

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    msg.info('This is %s' % sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info(
        "%s stopped at %s, trf exit code %d" % (
        sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


## Get the base transform with all arguments added
def getTransform():

    executorSet = set()
    executorSet.add(
        athenaExecutor(
            name='RAWtoESD',
            skeletonFile='RecJobTransforms/skeleton.RAWtoESD_tf.py',
            substep='r2e', inData=['BS', 'RDO'], outData=['NTUP_FTKIP'],
            perfMonFile = 'ntuple_RAWtoESD.pmon.gz'))

    ### from 64 TOWER CONFIG
    for subregion in range(subregions):
        pos = thistower * subregions + subregion
        executorSet.add(
            athenaExecutor(
                name='FTKFullSimulationBank{0:02d}'.format(thistower),
                skeletonFile='TrigFTKSim/skeleton.FTKStandaloneSim.py',
                substep='FTKTwr{0:02d}'.format(thistower),
                inData=['NTUP_FTKIP', 'TXT_FTKIP'],
                inputEventTest = False,disableMP=True,
                outData=['NTUP_FTKTMP'],
                extraRunargs={
                    'bankregion': [thistower],
                    'banksubregion': [subregion],
                    'outputNTUP_FTKTMPFile': 'tmp.NTUP_FTKTMP'},
                # Need to ensure that the correct subregion is used
                runtimeRunargs={
                    'bankregion': [thistower],
                    'subregions': [subregion],
                    'patternbankpath':
                    '[runArgs.patternbankspath[{0}]]'.format(pos),
                    'fitconstantspath':
                    '[runArgs.fitconstantspath[{0}]]'.format(thistower),
                    'fit711constantspath':
                    '[runArgs.fit711constantspath[{0}]]'.format(thistower),
                    'sectorpath':
                    '[runArgs.sectorspath[{0}]]'.format(thistower),
                    #'outputNTUP_FTKTMPFile':"'tmp.NTUP_FTKTMP'"}))
                    #'outputNTUP_FTKTMPFile':'runArgs.outputNTUP_FTKTMP_{0:02d}File'.format(thistower)}))
                    'outputNTUP_FTKTMPFile':'runArgs.outputNTUP_FTKTMPFile'}))

    # NOTE: WE DON'T DO MERGING HERE BECAUSE IT'S ONE SUBREGION! ONLY FINAL MERGE
    # add final merge for all the tower, generating a NTUP_FTK file
    executorSet.add(
        athenaExecutor(name="FTKSimulationMergeFinal",
            skeletonFile='TrigFTKSim/skeleton.FTKStandaloneMerge.py',
            substep = "FTKFinal",
            inputEventTest = False,disableMP=True,
            inData=['NTUP_FTKTMP', 'TXT_FTKIP'],
            outData=['NTUP_FTK'],
            runtimeRunargs={
                'inputNTUP_FTKTMPFile': "['tmp.NTUP_FTKTMP']",
                'MergeFromTowers': True,
                'FirstRegion': 0,
                'NBanks': 1,
                'TruthTrackTreeName': "'truthtracks'",
                'EvtInfoTreeName': "'evtinfo'",
                'UnmergedRoadFormatName': "'FTKRoadsStream22.'",
                'UnmergedFormatName': "'FTKTracksStream22.'",
                'SaveTruthTree': '1'}))

    executorSet.add(
        athenaExecutor(
            name='RDOFTKCreator',
            skeletonFile='TrigFTKSim/skeleton.RDO_FTK_Creator.py',
            substep='r2eFTK', inData=[('RDO','NTUP_FTK')],
            outData=['RDO_FTK'], inputEventTest = False,
            perfMonFile='ntuple_RDOFTKCreator.pmon.gz'))

    executorSet.add(
         athenaExecutor(
            name='AODFTKCreator',
            skeletonFile='TrigFTKSim/skeleton.AOD_FTK_Creator.py',
            substep='rFTK2aFTK', inData=[('RDO_FTK'),('BS_FTK')],
            outData=[('AOD')], inputEventTest = False,
            perfMonFile='ntuple_AODFTKCreator.pmon.gz'))

    executorSet.add(
        athenaExecutor(
            name='BSFTKCreator',
            skeletonFile='TrigFTKSim/skeleton.BS_FTK_Creator.py',
            substep='bs2bsFTK', inData=[('BS','NTUP_FTK')],
            outData=['BS_FTK'], inputEventTest = False,
            perfMonFile='ntuple_BSFTKCreator.pmon.gz'))


    trf = transform(executor=executorSet,
                    description='FTK simulation for 1 tower, with {0} '
                                'subregions merge and final merge.'.format(
                                    subregions))

    addTrigFTKAthenaOptions(trf.parser)
    addDetectorArguments(trf.parser)
    addTrigFTKSimOptions(trf.parser, nsubregions=subregions)
    addTrigFTKSimMergeOptions(trf.parser);
    addTrigFTKSimTFOptions(trf.parser)
    addTrigFTKSimRFOptions(trf.parser)
    addFTKSimulationArgs(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKSim',
                          'Fast tracker simulation options')
    parser.defineArgGroup('TrigFTKMerge',
                          'Fast tracker simulation merge options')

    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                            help='Bank region number', group='TrigFTKSim', nargs='+')

    parser.add_argument(
        '--UseTSPBank',
        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
        help='TSP bank utilisation',
        group='TrigFTKRoadFinder')

    parser.add_argument(
        '--UberFinalMerge',
        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
        help='Final merge is done fully across towers',
        group='TrigFTKMerge')

    parser.add_argument(
        '--inputNTUP_FTKTMP_{0:02d}File'.format(thistower),
        type=trfArgClasses.argFactory(
            trfArgClasses.argNTUPFile,
            runarg=True, io='input',
            type='ntup_ftkiptmp',
            treeNames='ftkdata'),
        help='FTK NTUP input file from tower {0}'.format(thistower),
        group='TrigFTKSim')

    # File handling
    parser.add_argument(
        '--inputRDOFile', nargs='+',
        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='input'),
        help='Input RDO file',
        group='Reco Files')
    # File handling
    parser.add_argument(
        '--inputBSFile', nargs='+',
        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
        help='Input BS file',
        group='BS Files')
    parser.add_argument(
        '--outputNTUP_FTKIPFile',
        type=trfArgClasses.argFactory(
            trfArgClasses.argNTUPFile, runarg=True,
            io='output', type='ntup_ftkip',
            treeNames='ftkhits'),
        help='FTK RDO file in ROOT format',
        group='TrigFTKSim')
    parser.add_argument(
        '--inputNTUP_FTKIPFile',
        type=trfArgClasses.argFactory(
            trfArgClasses.argNTUPFile, runarg=True,
            io='input', type='ntup_ftkip',
            treeNames='ftkhits'),
        help='FTK RDO file in ROOT format', group='TrigFTKSim', nargs='+')
    parser.add_argument(
        '--inputTXT_FTKIPFile',
        type=trfArgClasses.argFactory(
            trfArgClasses.argFTKIPFile, runarg=True,
            io='input', type='txt_ftkip'),
        help='Wrapper files (in .dat.bz2 format)',
        group='TrigFTKSim', nargs='+')

    # The following for testing only
    parser.add_argument(
        '--outputNTUP_FTKTMP_{0:02d}File'.format(thistower),
        type=trfArgClasses.argFactory(
            trfArgClasses.argNTUPFile, runarg=True, io='output',
            type='ntup_ftk', treeNames='ftkdata'),
        help='Subregion {0} merged FTK temp file'.format(thistower),
        group='TrigFTKMerge')
    
    for subregion in range(subregions):
        parser.add_argument(
            '--inputNTUP_FTKTMP_{0:02d}_{1}File'.format(thistower, subregion),
            type=trfArgClasses.argFactory(
                trfArgClasses.argNTUPFile, runarg=True, io='input',
                type='ntup_ftkiptmp', treeNames='ftkdata'),
            help='FTK NTUP file from tower {0} subregion {1} (for '
            'testing only)'.format(thistower, subregion),
            group='TrigFTKSim')
        parser.add_argument(
            '--outputNTUP_FTKTMP_{0:02d}_{1}File'.format(thistower, subregion),
            type=trfArgClasses.argFactory(
                trfArgClasses.argNTUPFile, runarg=True, io='output',
                type='ntup_ftkiptmp', treeNames='ftkdata'),
            help='FTK NTUP file from tower {0} subregion {1} '
            '(for testing only)'.format(thistower, subregion),
            group='TrigFTKSim')

    parser.add_argument(
        '--outputNTUP_FTKFile',
        type=trfArgClasses.argFactory(
            trfArgClasses.argNTUPFile, runarg=True,
            io='output', type='ntup_ftk',
            treeNames='ftkdata'),
        help='FTK merged tracks file',
        group='TrigFTKMerge')
    parser.add_argument(
        '--inputNTUP_FTKFile',
        type=trfArgClasses.argFactory(
            trfArgClasses.argNTUPFile, runarg=True,
            io='input', type='ntup_ftk',
            treeNames='ftkdata'),
        help='FTK merged tracks file',
        group='TrigFTKMerge')

    parser.add_argument(
        '--outputRDO_FTKFile','--outputRDOFile',
        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='output'),
        help='Output RDO_FTK file',
        group='Reco Files')

    parser.add_argument(
        '--outputBS_FTKFile','--outputBSFile',
        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output'),
        help='Output BS_FTK file',
        group='BS Files')
 
    parser.add_argument(
        '--outputAODFile', nargs='+',
        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='output'),
        help='Output AOD file',
        group='Reco Files')


if __name__ == '__main__':
    main()
