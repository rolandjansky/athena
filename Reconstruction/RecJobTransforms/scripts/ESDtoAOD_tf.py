#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## A simple ESDtoAOD transform. 
# @version $Id: ESDtoAOD_tf.py 670203 2015-05-27 13:19:01Z graemes $ 

import os.path
import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

ListOfDefaultPositionalKeys=['--DBRelease', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--geometryVersion', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputESDFile', '--maxEvents', '--noimf', '--notcmalloc', '--outputAODFile', '--outputHIST_AOD_INTFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation']

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
    trf = transform(executor = athenaExecutor(name = 'ESDtoAOD', skeletonFile = 'RecJobTransforms/skeleton.ESDtoAOD_tf.py',
                                              substep = 'e2a'))
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('ESDtoAOD_tf', 'ESDtoAOD specific options')
    parser.add_argument('--inputESDFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input ESD file', group='ESDtoAOD_tf')
    parser.add_argument('--outputAODFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output AOD file', group='ESDtoAOD_tf')
    parser.add_argument('--outputHIST_AOD_INTFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', countable=False),
                        help='Output DQ monitoring file', group='ESDtoAOD_tf')


if __name__ == '__main__':
    main()
