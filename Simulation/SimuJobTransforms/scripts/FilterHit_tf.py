#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Run Geant4 simulation on generated events and produce a HITS file.
"""

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
from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addCosmicsTrfArgs, addCommonSimTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

# Prodsys hack...
ListOfDefaultPositionalKeys=['--DBRelease', '--TruthReductionScheme', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputHITSFile', '--maxEvents', '--outputHITS_FILTFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--tcmalloc']

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
    trf = transform(executor = athenaExecutor(name = 'FilterHitTf', substep="filthits", skeletonFile = 'SimuJobTransforms/skeleton.FilterHit.py',
                                              tryDropAndReload = False))
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('FilterHit_tf', 'FilterHit_tf specific options')
    parser.add_argument('--inputHITSFile', '--inputHitsFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='hits'),
                        help='Input HITS file', group='FilterHit_tf')
    parser.add_argument('--outputHITS_FILTFile', '--outputHits_FILTFile', '--outputHITSFile', '--outputHitsFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', runarg=True, type='hits'),
                        help='Output HITS file', group='FilterHit_tf')
    parser.add_argument('--TruthReductionScheme',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), help='Truth reduction method to be used.', group='FilterHit_tf')

## FIXME - not sure what the equivalent of the method below is in the new framework?

##     def doPreRunActions(self):
##         JobTransform.doPreRunActions(self)
##         if hasattr(self,'_maxEventsStrategy'):
##             self._maxEventsStrategy = 'INPUTEVENTS'
##         else:
##             print "WARNING EVGENtoHITJobTransform has no attribute \'_maxEventsStrategy\'."


if __name__ == '__main__':
    main()
