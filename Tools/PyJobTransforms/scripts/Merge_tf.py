#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## Merge_tf.py - Transform for merging any data type

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, hybridPOOLMergeExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs
from PyJobTransforms.trfExe import DQMergeExecutor
from PyJobTransforms.trfExe import tagMergeExecutor
from PyJobTransforms.trfExe import bsMergeExecutor
from PyJobTransforms.trfArgs import addD3PDArguments, addExtraDPDTypes
from PATJobTransforms.PATTransformUtils import addNTUPMergeSubsteps, addPhysValidationMergeFiles
from PATJobTransforms.PATTransformUtils import addDAODArguments, addDAODMergerSubsteps

import PyJobTransforms.trfArgClasses as trfArgClasses

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():

    msg.info('This is %s', sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d", sys.argv[0], time.asctime(), trf.exitCode)
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
    executorSet.add(hybridPOOLMergeExecutor(name = 'ESDMerge', skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                                   inData = ['ESD'], outData = ['ESD_MRG']))
    executorSet.add(hybridPOOLMergeExecutor(name = 'AODMerge', skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                                   inData = ['AOD'], outData = ['AOD_MRG']))
    executorSet.add(athenaExecutor(name = 'AODtoTAG', skeletonFile = 'RecJobTransforms/skeleton.AODtoTAG_tf.py',
                                   inData = ['AOD_MRG'], outData = ['TAG'],))
    executorSet.add(tagMergeExecutor(name = 'TAGFileMerge', exe = 'CollAppend', inData = set(['TAG']), outData = set(['TAG_MRG'])))
    executorSet.add(DQMergeExecutor(name = 'DQHistogramMerge', inData = [('HIST_ESD', 'HIST_AOD'), 'HIST'], outData = ['HIST_MRG']))
    executorSet.add(athenaExecutor(name = 'RDOMerge', skeletonFile = 'RecJobTransforms/skeleton.MergeRDO_tf.py',
                                   inData = ['RDO'], outData = ['RDO_MRG']))
    executorSet.add(bsMergeExecutor(name = 'RAWFileMerge', exe = 'file_merging', inData = set(['BS']), outData = set(['BS_MRG'])))
    executorSet.add(athenaExecutor(name = 'EVNTMerge', skeletonFile = 'PyJobTransforms/skeleton.EVNTMerge.py',inData = ['EVNT'], outData = ['EVNT_MRG']))

    addDAODMergerSubsteps(executorSet)
    addNTUPMergeSubsteps(executorSet)

    trf = transform(executor = executorSet)

    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addMyArgs(trf.parser)

    addDAODArguments(trf.parser)
    addPhysValidationMergeFiles(trf.parser)
    addD3PDArguments(trf.parser, transform=trf, addD3PDMRGtypes=True)
    addExtraDPDTypes(trf.parser, transform=trf, NTUPMergerArgs = True)

    # Add HITSMerge only if SimuJobTransforms is available
    try:
        from SimuJobTransforms.SimTransformUtils import addHITSMergeArguments
        addHITSMergeArguments(trf.parser)
        simStepSet = set()
        simStepSet.add(athenaExecutor(name = 'HITSMerge', substep="hitsmerge", skeletonFile = 'SimuJobTransforms/skeleton.HITSMerge.py',
                                   tryDropAndReload = False, inData = ['HITS'], outData = ['HITS_MRG']))
        trf.appendToExecutorSet(list(simStepSet)[0])
    except ImportError as e:
        msg.warning('Failed to import simulation arguments ({0}). HITSMerge will not be available.'.format(e))


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

    parser.defineArgGroup('TAGMerge_tf', 'TAG merging specific options')
    parser.add_argument('--inputTAGFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='input'),
                        help='Input TAG file(s)', group='TAGMerge_tf')
    parser.add_argument('--outputTAG_MRGFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='output'),
                        help='Output merged TAG file', group='TAGMerge_tf')

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

    parser.defineArgGroup('RAWMerge_tf', 'RAWMerge specific options')
    parser.add_argument('--inputBSFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                        help='Input BS file(s)', group='RAWMerge_tf')
    parser.add_argument('--outputBS_MRGFile', '--outputBSFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output'),
                        help='Output merged BS file (best if this file ends in ._0001.data, but see allowRename option below)', 
                        group='RAWMerge_tf')
    parser.add_argument('--maskEmptyInputs', type=trfArgClasses.argFactory(trfArgClasses.argBool), group='RAWMerge_tf',
                        help='If true then empty BS files are not included in the merge (default True)', 
                        default=trfArgClasses.argBool('True'))
    parser.add_argument('--allowRename', type=trfArgClasses.argFactory(trfArgClasses.argBool), group='RAWMerge_tf',
                        help='If true merged BS file will be forcibly renamed to the value of "outputBSFile" (default True)', 
                        default=trfArgClasses.argBool('True'))

    parser.defineArgGroup('EVNTMerge_tf', 'EVNT merge job specific options')
    parser.add_argument('--inputEVNTFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='input', runarg=True, type='evnt'),
                        help='Input EVNT file', group='EVNTMerge_tf')
    parser.add_argument('--outputEVNT_MRGFile', '--outputEVNTFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='output', runarg=True, type='evnt'),
                        help='Output merged EVNT file', group='EVNTMerge_tf')
    parser.add_argument('--eventService', type=trfArgClasses.argFactory(trfArgClasses.argBool), metavar = "BOOL",
                        help='Switch AthenaMP to the Event Service configuration', group='EVNTMerge_tf')

    parser.add_argument('--fastPoolMerge', type=trfArgClasses.argFactory(trfArgClasses.argBool),
                        help='Hybrid POOL merging switch (default True)')


if __name__ == '__main__':
    main()
