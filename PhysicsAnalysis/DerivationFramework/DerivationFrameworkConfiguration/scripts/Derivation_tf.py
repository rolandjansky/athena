#!/usr/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Run DAOD building from AOD or EVNT, or D2AOD building from DAOD, using the derivation framework
"""

import sys
import time

from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.transform import transform

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])


# Prodsys hack...
# Not clear what this is for.... maybe not needed?
#ListOfDefaultPositionalKeys = ['--AMIConfig', '--AMITag', '--CA', '--DBRelease', '--argJSON', '--asetup', '--athena', '--athenaMPEventsBeforeFork', '--athenaMPMergeTargetSize', '--athenaMPStrategy', '--athenaMPUseEventOrders', '--athenaopts', '--attempt', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--detectors', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--eventService', '--execOnly', '--fSampltag', '--fileValidation', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputBS_SKIMFile', '--inputFileValidation', '--inputHITSFile', '--inputRDO_BKGFile', '--jobid', '--maxEvents', '--multiprocess', '--multithreaded', '--outputFileValidation', '--outputRDOFile', '--outputRDO_FILTFile', '--outputRDO_SGNLFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--runNumber', '--samplingFractionDbTag', '--sharedWriter', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--steering', '--taskid', '--tcmalloc', '--triggerConfig', '--valgrind', '--valgrindDefaultOpts', '--valgrindExtraOpts']


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
    from DerivationFrameworkConfiguration.DerivationTransformHelpers import addDerivationArguments, addDerivationSubstep
    addDerivationSubstep(executor_set)
    trf = transform(executor=executor_set,
                    description='ATLAS derivation framework transform. Inputs must be EVNT, AOD or DAOD. Outputs must be DAOD or D2AOD.')
    addAthenaArguments(trf.parser)
    addDerivationArguments(trf.parser)
    return trf


if __name__ == '__main__':
    main()
