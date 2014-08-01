#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Small test transform which only runs HIST merging
# @version $Id: HISTMerge_tf.py 603486 2014-06-25 09:07:28Z graemes $ 

import os.path
import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import DQMergeExecutor
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses

ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--checkEventCount', '--dumpPickle', '--env', '--execOnly', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputHISTFile', '--inputHIST_AODFile', '--inputHIST_ESDFile', '--orphanKiller', '--outputHIST_MRGFile', '--reportName', '--reportType', '--showGraph', '--showPath', '--showSteps', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--tcmalloc']

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
    executorSet = DQMergeExecutor(name = 'DQHistogramMerge', inData = [('HIST_ESD', 'HIST_AOD'), 'HIST'], outData = ['HIST_MRG'])
    
    trf = transform(executor = executorSet)
    
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
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


if __name__ == '__main__':
    main()
