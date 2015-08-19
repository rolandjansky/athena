#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Stopped particle simulation
# @version $Id: Reco_tf.py 529035 2012-12-05 15:45:24Z graemes $

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, DQMergeExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addCommonSimTrfArgs, addCommonSimDigTrfArgs


import PyJobTransforms.trfArgClasses as trfArgClasses

# Prodsys hack...
ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--DBRelease', '--DataRunNumber', '--FwdRegionOn', '--LucidOn', '--ZDCOn', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--enableLooperKiller', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--firstEvent', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputEVNTFile', '--jobNumber', '--maxEvents', '--outputEvgen_StoppedFile', '--outputHITSFile', '--physicsList', '--postExec', '--postInclude', '--preExec', '--preInclude', '--randomSeed', '--reportName', '--reportType', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--tcmalloc', '--useISF']

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
    executorSet.add(athenaExecutor(name = 'SPGenerator', skeletonFile = 'SimuJobTransforms/skeleton.EVGENtoHIT_MC12.py',
                                   inData = ['EVNT'], outData = ['EVNT_STOPPED'], tryDropAndReload = False))
    executorSet.add(athenaExecutor(name = 'SPSim', skeletonFile = 'SimuJobTransforms/skeleton.EVGENtoHIT_MC12.py',
                                   inData = ['EVNT_STOPPED'], outData = ['HITS'], tryDropAndReload = False))

    trf = transform(executor = executorSet)

    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonSimTrfArgs(trf.parser)
    addCommonSimDigTrfArgs(trf.parser)
    addMyArgs(trf.parser)
    addForwardDetTrfArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('StoppedG4', 'Specific G4 options')

    parser.add_argument('--outputEVNT_STOPPEDFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', runarg=True, type='evnt'),
                        help='Output evgen file with stopped particles', group='StoppedG4')

## FIXME - not sure what the equivalent of the method below is in the new framework?

##     def doPreRunActions(self):
##         JobTransform.doPreRunActions(self)
##         if hasattr(self,'_maxEventsStrategy'):
##             self._maxEventsStrategy = 'ABORT'
##         else:
##             print "WARNING EVGENtoHITJobTransform has no attribute \'_maxEventsStrategy\'."

if __name__ == '__main__':
    main()
