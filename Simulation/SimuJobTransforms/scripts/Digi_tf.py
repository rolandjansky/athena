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
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments, addTriggerArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addBasicDigiArgs, addPileUpTrfArgs, addCommonSimDigTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

# Prodsys hack...
ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--AddCaloDigi', '--DBRelease', '--DataRunNumber', '--FwdRegionOn', '--LucidOn', '--PileUpPremixing', '--ZDCOn', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--beamType', '--bunchSpacing', '--checkEventCount', '--command', '--conditionsTag', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--doAllNoise', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputBeamGasHitsFile', '--inputBeamHaloHitsFile', '--inputCavernHitsFile', '--inputHITSFile', '--inputHighPtMinbiasHitsFile', '--inputLowPtMinbiasHitsFile', '--jobNumber', '--maxEvents', '--numberOfBeamGas', '--numberOfBeamHalo', '--numberOfCavernBkg', '--numberOfHighPtMinBias', '--numberOfLowPtMinBias', '--outputRDOFile', '--outputRDO_FILTFile', '--pileupFinalBunch', '--pileupInitialBunch', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--runNumber', '--samplingFractionDbTag', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--tcmalloc', '--testPileUpConfig', '--triggerConfig']

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
    from SimuJobTransforms.SimTransformUtils import addDigitizationSubstep, addDigitizationArguments
    addDigitizationSubstep(executorSet)
    trf = transform(executor = executorSet, description = 'ATLAS Digitization transform. Inputs must be HITS. Outputs must be RDO.')
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser, addTrigFilter=False)
    addDigitizationArguments(trf.parser)
    return trf

## FIXME - not sure what the equivalent of the method below is in the new framework?

##     def doPreRunActions(self):
##         JobTransform.doPreRunActions(self)
##         if hasattr(self,'_maxEventsStrategy'):
##             self._maxEventsStrategy = 'INPUTEVENTS'
##         else:
##             print "WARNING EVGENtoHITJobTransform has no attribute \'_maxEventsStrategy\'."

if __name__ == '__main__':
    main()
