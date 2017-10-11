#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## DAODMerge_tf.py - Dervied AOD merger capable of merging different flavours
#  of DAOD in the one job
# @version $Id: DAODMerge_tf.py 621674 2014-10-14 11:35:59Z graemes $ 

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

from PATJobTransforms.PATTransformUtils import addDAODArguments, addDAODMergerSubsteps

import PyJobTransforms.trfArgClasses as trfArgClasses

ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--DBRelease', '--argJSON', '--asetup', '--athena', '--athenaMPMergeTargetSize', '--athenaopts', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputDAOD_HIGGS3D1File', '--inputDAOD_HIGGS4D2File', '--inputDAOD_HIGGS5D1File', '--inputDAOD_HIGGS8D1File', '--inputDAOD_HIGGS8D2File', '--inputDAOD_JETMET1File', '--inputDAOD_JETMET2File', '--inputDAOD_JETMET3File', '--inputDAOD_JETMET4File', '--inputDAOD_JETMET5File', '--inputDAOD_SUSY1File', '--inputDAOD_TAU1File', '--inputDAOD_TAU3File', '--inputDAOD_TEST1File', '--inputDAOD_TEST2File', '--inputDAOD_TEST3File', '--inputDAOD_TEST4File', '--inputDAOD_TEST5File', '--inputDAOD_TOP1File', '--maxEvents', '--orphanKiller', '--outputDAOD_HIGGS3D1_MRGFile', '--outputDAOD_HIGGS4D2_MRGFile', '--outputDAOD_HIGGS5D1_MRGFile', '--outputDAOD_HIGGS8D1_MRGFile', '--outputDAOD_HIGGS8D2_MRGFile', '--outputDAOD_JETMET1_MRGFile', '--outputDAOD_JETMET2_MRGFile', '--outputDAOD_JETMET3_MRGFile', '--outputDAOD_JETMET4_MRGFile', '--outputDAOD_JETMET5_MRGFile', '--outputDAOD_SUSY1_MRGFile', '--outputDAOD_TAU1_MRGFile', '--outputDAOD_TAU3_MRGFile', '--outputDAOD_TEST1_MRGFile', '--outputDAOD_TEST2_MRGFile', '--outputDAOD_TEST3_MRGFile', '--outputDAOD_TEST4_MRGFile', '--outputDAOD_TEST5_MRGFile', '--outputDAOD_TOP1_MRGFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--tcmalloc', '--trigStream']

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
    addDAODMergerSubsteps(executorSet)

    trf = transform(executor = executorSet)

    addMyArguments(trf.parser)
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addDAODArguments(trf.parser)
    return trf

def addMyArguments(parser):
    parser.defineArgGroup('Merge', 'Merge strategy options')
    parser.add_argument('--fastPoolMerge', type=trfArgClasses.argFactory(trfArgClasses.argBool), metavar="BOOL",
                        help='Hybrid POOL merging switch (default False)', group='Merge')

if __name__ == '__main__':
    main()
