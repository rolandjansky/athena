#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Run overlay between either pre-mixed MC background RDO or zero-bias data RDO and hard scatter HITS
"""

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments, addTriggerArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

# Prodsys hack...
ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--AMIConfig', '--AMITag', '--AddCaloDigi', '--DBRelease', '--FwdRegionOn', '--LucidOn', '--ReadByteStream', '--ZDCOn', '--argJSON', '--asetup', '--athena', '--athenaMPMergeTargetSize', '--athenaopts', '--attempt', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--digiSteeringConf', '--doAllNoise', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--fSampltag', '--fileValidation', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputFileValidation', '--inputHITSFile', '--inputRDO_BKGFile', '--jobid', '--maxEvents', '--orphanKiller', '--outputFileValidation', '--outputRDOFile', '--outputRDO_FILTFile', '--outputRDO_SGNLFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--runNumber', '--samplingFractionDbTag', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--taskid', '--tcmalloc', '--tmpRDO', '--tmpRDO_FILT', '--triggerConfig', '--valgrind', '--valgrindbasicopts', '--valgrindextraopts']

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
    from EventOverlayJobTransforms.overlayTransformUtils import addOverlayArguments, addOverlaySubstep
    addOverlaySubstep(executorSet)
    trf = transform(executor = executorSet, description = 'ATLAS Overlay transform. Inputs must be HITS. Outputs must be RDO.')
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser, addTrigFilter=False)
    addOverlayArguments(trf.parser)
    return trf

if __name__ == '__main__':
    main()
