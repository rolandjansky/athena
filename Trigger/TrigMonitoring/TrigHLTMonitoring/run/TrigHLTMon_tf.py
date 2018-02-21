#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = """JobTransform to run TrigHLTMonitoring standalone jobs"""


import sys
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor,DQMergeExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
import PyJobTransforms.trfArgClasses as trfArgClasses

if __name__ == '__main__':

    executorSet = set()
    executorSet.add(athenaExecutor(name = 'HLTMon', skeletonFile = 'TrigHLTMonitoring/skeleton.HLTMon_tf.py',
                                   substep = 'r2a', inData = ['BS','AOD'], outData = ['HIST_TEMP']))
    executorSet.add(DQMergeExecutor(name = 'DQHistogramMerge', inData = ['HIST_TEMP'], outData = ['HIST']))
    trf = transform(executor = executorSet)
    addAthenaArguments(trf.parser)
    trf.parser.defineArgGroup('TrigHLTMon args', 'HLT Monitoring transform arguments')
    trf.parser.add_argument('--inputBSFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                            help='Input bytestream file', group='TrigHLTMon args')

    trf.parser.add_argument('--inputAODFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                            help='Input pool file', group='TrigHLTMon args')

    trf.parser.add_argument('--outputHISTFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output'),
                            help='Output HIST file', group='TrigHLTMon args')

    trf.parser.add_argument('--useDB',type=trfArgClasses.argFactory(trfArgClasses.argBool),help='Use TrigConfigSvc',)

    trf.parser.defineArgGroup('Monitoring args', 'Monitoring options')
    trf.parser.add_argument('--monFlags', nargs='+',type=trfArgClasses.argFactory(trfArgClasses.argList),
            help='TrigHLTMon histogram switches', group='Monitoring args')
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()
