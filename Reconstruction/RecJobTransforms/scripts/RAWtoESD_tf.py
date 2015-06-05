#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## A simple RAWtoESD transform. 
# @version $Id: RAWtoESD_tf.py 670203 2015-05-27 13:19:01Z graemes $ 

import os.path
import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addD3PDArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses

ListOfDefaultPositionalKeys = ['--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--checkEventCount', '--command', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputBSFile', '--inputRDOFile', '--maxEvents', '--noimf', '--notcmalloc', '--outputESDFile', '--outputHIST_ESD_INTFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--outputTXT_JIVEXMLTGZFile']

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
    trf = transform(executor = athenaExecutor(name = 'RAWtoESD', skeletonFile = 'RecJobTransforms/skeleton.RAWtoESD_tf.py',
                                              substep = 'r2e',
                                              ))
    addAthenaArguments(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('RAWtoESD_tf', 'RAW to ESD specific options')
    parser.add_argument('--inputRDO_TRIGFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argRDO_TRIGFile, io='input'),
                        help='Input rdo trigger file', group='RAWtoESD_tf')
    parser.add_argument('--inputBSFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                        help='Input bytestream file', group='RAWtoESD_tf')
    parser.add_argument('--inputRDOFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input RDO file', group='RAWtoESD_tf')
    parser.add_argument('--outputESDFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output ESD file', group='RAWtoESD_tf')
    parser.add_argument('--outputHIST_ESD_INTFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', countable=False),
                        help='Output DQ monitoring file', group='RAWtoESD_tf')
    parser.add_argument('--outputTXT_JIVEXMLTGZFile', type = trfArgClasses.argFactory(trfArgClasses.argFile, io = 'output'),
                        help = 'Output JiveXML.tgz file', group = 'RAWtoESD_tf')


if __name__ == '__main__':
    main()
