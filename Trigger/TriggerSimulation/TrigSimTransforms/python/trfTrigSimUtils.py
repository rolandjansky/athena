# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from PyJobTransforms.trfArgClasses import argFactory, argBSFile, argPOOLFile
from PyJobTransforms.trfExe import athenaExecutor

## @brief Add trigger simulation substeps
def addTrigSimSubsteps(executorSet):

    executorSet.add( athenaExecutor( name = 'RDOtoBS', skeletonFile = 'TrigSimTransforms/skeleton.RDOtoBS.py',
                                     substep = 'r2b', inData = ['RDO'], outData = ['BS_OLD']
                                     ))

    executorSet.add( athenaExecutor( name = 'BStoTRIGBS', skeletonFile = 'TrigSimTransforms/skeleton.BStoTRIGBS.py',
                                     substep = 'b2tb', inData = ['BS_OLD'], outData = ['BS_TRIG']
                                     ))

    executorSet.add( athenaExecutor( name = 'TRIGBStoRDO', skeletonFile = 'TrigSimTransforms/skeleton.TRIGBStoRDO.py',
                                     substep = 'tb2tr', inData = [('BS_TRIG','RDO')], outData = ['RDO_TRIG']
                                     ))



## @brief Add trigger simulation related arguments
def addTrigSimArguments(parser):
    parser.defineArgGroup('Trigger', 'Trigger Related Options')

    # input and output
    parser.add_argument('--outputBS_OLDFile',
                        type = argFactory( argBSFile, io='output'),
                        help = 'Output BS file after transforming to legacy payload',
                        group = 'Trigger')

    parser.add_argument('--inputBS_OLDFile',
                        type = argFactory( argBSFile, io='input'),
                        help = 'Input BS file with legacy payload',
                        group = 'Trigger')

    parser.add_argument('--outputBS_TRIGFile',
                        type = argFactory( argBSFile, io='output'),
                        help = 'Output BS file after trigger simulation',
                        group = 'Trigger')

    parser.add_argument('--inputBS_TRIGFile',
                        type = argFactory( argBSFile, io='input'),
                        help = 'Input BS file with trigger data',
                        group = 'Trigger')

    parser.add_argument('--outputRDO_TRIGFile',
                        type = argFactory( argPOOLFile, io='output'),
                        help = 'Output RDO file after trigger and merging',
                        group = 'Trigger')

