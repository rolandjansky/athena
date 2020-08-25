#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Run overlay between either pre-mixed MC background RDO or zero-bias data RDO and hard scatter HITS
"""

import sys
import time

from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments, addTriggerArguments
from PyJobTransforms.transform import transform

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])


# Prodsys hack...
ListOfDefaultPositionalKeys = ['--AMIConfig', '--AMITag', '--AddCaloDigi', '--CA', '--DBRelease', '--PileUpPremixing', '--argJSON', '--asetup', '--athena', '--athenaMPEventsBeforeFork', '--athenaMPMergeTargetSize', '--athenaMPStrategy', '--athenaMPUseEventOrders', '--athenaopts', '--attempt', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--detectors', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--digiSteeringConf', '--doAllNoise', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--eventService', '--execOnly', '--fSampltag', '--fileValidation', '--geometryVersion', '--ignoreErrors', '--ignoreFiles',
                               '--ignorePatterns', '--imf', '--inputBS_SKIMFile', '--inputFileValidation', '--inputHITSFile', '--inputRDO_BKGFile', '--jobid', '--maxEvents', '--multiprocess', '--multithreaded', '--outputFileValidation', '--outputRDOFile', '--outputRDO_FILTFile', '--outputRDO_SGNLFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--runNumber', '--samplingFractionDbTag', '--sharedWriter', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--steering', '--taskid', '--tcmalloc', '--triggerConfig', '--valgrind', '--valgrindDefaultOpts', '--valgrindExtraOpts']


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    msg.info('This is %s', sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info('%s stopped at %s, trf exit code %d',
             sys.argv[0], time.asctime(), trf.exitCode)
    sys.exit(trf.exitCode)


def getTransform():
    executor_set = set()
    from OverlayConfiguration.OverlayTransformHelpers import addOverlayArguments, addOverlaySubstep
    addOverlaySubstep(executor_set)
    trf = transform(executor=executor_set,
                    description='ATLAS Overlay transform. Inputs must be HITS. Outputs must be RDO.')
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser, addTrigFilter=False)
    addOverlayArguments(trf.parser)
    return trf


if __name__ == '__main__':
    main()
