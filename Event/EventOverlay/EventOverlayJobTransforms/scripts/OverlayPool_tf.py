#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Overlay simulated physics events into zero bias real data events; the real data events are in POOL format."""

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

import PyJobTransforms.trfArgClasses as trfArgClasses

# Prodsys hack...
ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--AMIConfig', '--AMITag', '--AddCaloDigi', '--DBRelease', '--FwdRegionOn', '--LucidOn', '--ReadByteStream', '--ZDCOn', '--argJSON', '--asetup', '--athena', '--athenaMPMergeTargetSize', '--athenaopts', '--attempt', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--digiSteeringConf', '--doAllNoise', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--fSampltag', '--fileValidation', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputFileValidation', '--inputHITSFile', '--inputRDO_BKGFile', '--jobid', '--maxEvents', '--orphanKiller', '--outputFileValidation', '--outputRDOFile', '--outputRDO_FILTFile', '--outputRDO_SGNLFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--runNumber', '--samplingFractionDbTag', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--taskid', '--tcmalloc', '--tmpRDO', '--tmpRDO_FILT', '--triggerConfig', '--valgrind', '--valgrindbasicopts', '--valgrindextraopts']

@stdTrfExceptionHandler
@sigUsrStackTrace

def main():

    msg.info('This is %s' % sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    if 'outputFileValidation' in  trf._argdict and  trf._argdict['outputFileValidation'].value is False:
        msg.info('Skipping report generation')
    else:
        trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
    from EventOverlayJobTransforms.overlayTransformUtils import addOverlay_PoolSubstep, addOverlay_PoolArguments
    addOverlay_PoolSubstep(executorSet)
    trf = transform(executor = executorSet, description = 'ATLAS Overlay transform. Inputs must be HITS. Outputs must be RDO.')
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser, addTrigFilter=False)
    addOverlay_PoolArguments(trf.parser)
    return trf

if __name__ == '__main__':
    main()

# old skeleton='EventOverlayJobTransforms/skeleton.overlay.py'
