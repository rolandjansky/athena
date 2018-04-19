#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Merge_tf.py - Transform for merging any data types

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, hybridPOOLMergeExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs
from PyJobTransforms.trfExe import DQMergeExecutor
from PyJobTransforms.trfExe import tagMergeExecutor
from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs

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

def getTransform():
    executorSet = set()
    executorSet.add(hybridPOOLMergeExecutor(name = 'ESDMerge', skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                                   inData = ['ESD'], outData = ['ESD_MRG']))
    executorSet.add(hybridPOOLMergeExecutor(name = 'AODMerge', skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                                   inData = ['AOD'], outData = ['AOD_MRG']))
    executorSet.add(athenaExecutor(name = 'AODtoTAG', skeletonFile = 'RecJobTransforms/skeleton.AODtoTAG_tf.py',
                                   inData = ['AOD_MRG'], outData = ['TAG'],))
    executorSet.add(DQMergeExecutor(name = 'DQHistogramMerge', inData = [('HIST_ESD', 'HIST_AOD'), 'HIST'], outData = ['HIST_MRG']))
    executorSet.add(athenaExecutor(name = 'RDOMerge', skeletonFile = 'RecJobTransforms/skeleton.MergeRDO_tf.py',
                                   inData = ['RDO'], outData = ['RDO_MRG']))

    #executorSet.add(athenaExecutor(name = 'Merge', skeletonFile = 'PyJobTransforms/skeleton.Merge.py',inData = ['EVNT'], outData = ['EVNT_MRG']))

    trf = transform(executor = executorSet)

    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('ESDMerge_tf', 'ESD merge job specific options')
    parser.add_argument('--inputESDFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input ESD file', group='ESDMerge_tf')
    parser.add_argument('--outputESD_MRGFile', '--outputESDFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output merged ESD file', group='ESDMerge_tf')

    parser.defineArgGroup('AODMerge_tf', 'AOD merge job specific options')
    parser.add_argument('--inputAODFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', type='aod'),
                        help='Input AOD file', group='AODMerge_tf')
    parser.add_argument('--inputAOD_MRGFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', type='aod'),
                        help='Input (premarged) AOD file', group='AODMerge_tf')
    parser.add_argument('--outputAOD_MRGFile', '--outputAODFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', type='aod'),
                        help='Output merged AOD file', group='AODMerge_tf')
    parser.add_argument('--outputTAGFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='output', type='tag'), 
                        help='Output TAG file', group='AODMerge_tf')

    parser.defineArgGroup('DQHistMerge_tf', 'DQ merge specific options')
    parser.add_argument('--inputHISTFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='input', runarg=True, type='hist'), 
                        help='Input DQ HIST file', group='DQHistMerge_tf')
    parser.add_argument('--inputHIST_AODFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='input', runarg=True, type='hist_aod',
                                                      countable=False), 
                        help='Input DQ AOD step monitoring file', group='DQHistMerge_tf')
    parser.add_argument('--inputHIST_ESDFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='input', runarg=True, type='hist_esd',
                                                      countable=False), 
                        help='Input DQ AOD step monitoring file', group='DQHistMerge_tf')
    parser.add_argument('--outputHIST_MRGFile', '--outputHISTFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, type='hist'), 
                        help='Output DQ monitoring file', group='DQHistMerge_tf')

    parser.defineArgGroup('RDOMerge_tf', 'RDO merge job specific options')
    parser.add_argument('--inputRDOFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='input'),
                        help='Input RDO file', group='RDOMerge_tf')
    parser.add_argument('--outputRDO_MRGFile', '--outputRDOFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='output'),
                        help='Output merged RDO file', group='RDOMerge_tf')


if __name__ == '__main__':
    main()
