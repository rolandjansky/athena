# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Module whichde defines transform steps and arguments specific
to Trigger simulation.
"""

from PyJobTransforms.trfArgClasses import argFactory, argBSFile, argPOOLFile, argSubstep, argSubstepList
from PyJobTransforms.trfExe import athenaExecutor
from .trfTrigSimExe import athenaExecutorJO

# # @brief Add trigger simulation substeps
def addTrigSimSubsteps(executorSet):

    executorSet.add(athenaExecutor(name='RDOtoBSOLD', skeletonFile='TrigSimTransforms/skeleton.RDOtoBS.py',
                                   substep='r2bo', inData=['RDO'], outData=['BS_OLD']
                                   ))

    executorSet.add(athenaExecutorJO(name='BStoTRIGBS', skeletonFile='TrigSimTransforms/skeleton.BStoTRIGBS.py',
                                     substep='b2tb', inData=['BS_OLD'], outData=['BS_TRIG_OLD']
                                     ))

    executorSet.add(athenaExecutor(name='TRIGBStoRDO', skeletonFile='TrigSimTransforms/skeleton.TRIGBStoRDO.py',
                                   substep='tb2tr', inData=[('BS_TRIG_OLD', 'RDO')], outData=['RDO_TRIG_OLD']
                                   ))


# # @brief Add trigger simulation related arguments
def addTrigSimArguments(parser):
    parser.defineArgGroup('LegacyTrigger', 'Legacy Trigger Simulation Related Options')

    # input and output
    parser.add_argument('--outputBS_OLDFile',
                        type=argFactory(argBSFile, io='output'),
                        help='Output BS file after transforming to legacy payload',
                        group='LegacyTrigger')

    parser.add_argument('--inputBS_OLDFile',
                        type=argFactory(argBSFile, io='input'),
                        help='Input BS file with legacy payload',
                        group='LegacyTrigger')

    parser.add_argument('--outputBS_TRIG_OLDFile',
                        type=argFactory(argBSFile, io='output'),
                        help='Output BS file after legacy trigger simulation',
                        group='LegacyTrigger')

    parser.add_argument('--inputBS_TRIG_OLDFile',
                        type=argFactory(argBSFile, io='input'),
                        help='Input BS file with legacy trigger data',
                        group='LegacyTrigger')

    parser.add_argument('--outputRDO_TRIG_OLDFile',
                        type=argFactory(argPOOLFile, io='output'),
                        help='Output RDO file after legacy trigger simulation and merging with RDO',
                        group='LegacyTrigger')

    parser.add_argument('--copyJO', group = 'LegacyTrigger',
                        type=argFactory(argSubstepList, splitter=','), nargs='+',
                        metavar='substep:PATH,PATH,...',
                        help='Specifies one or more JO files to copy from current release, '
                        'can be used with steps specifying different release')

    parser.add_argument('--outputBSEformatVersion', group='LegacyTrigger',
                        type=argFactory(argSubstep), nargs='+',
                        metavar='substep:VERSION',
                        help='Specifies eformat version name for ByteStream output, use "run1" to '
                        'produce run1-compatible output.')
    parser.add_argument('--outputBSEventStorageVersion', group='LegacyTrigger',
                        type=argFactory(argSubstep), nargs='+',
                        metavar='substep:VERSION',
                        help='Specifies EventStorage version name for ByteStream output, use "run1" to '
                        'produce run1-compatible output.')
