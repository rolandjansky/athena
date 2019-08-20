#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Transform for preparation of BS overlay
# @version $Id: BSOverlayFilter_tf.py 733403 2016-04-01 18:38:53Z ahaas $ 

import os.path
import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from EventOverlayJobTransforms.overlayTransformUtils import BSJobSplitterExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--argJSON', '--asetup', '--athena', '--athenaMPMergeTargetSize', '--athenaopts', '--attempt', '--checkEventCount', '--command', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--eventIdFile', '--execOnly', '--fileValidation', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputFileValidation', '--inputZeroBiasBSFile', '--inputFilterFile', '--jobNumber', '--jobid', '--lumiBlockMapFile', '--maxEvents', '--maxFilesPerSubjob', '--orphanKiller', '--outputBS_SKIMFile', '--outputFileValidation', '--overlayConfigFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--taskid', '--tcmalloc', '--triggerBit', '--valgrind', '--valgrindbasicopts', '--valgrindextraopts']

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
    from EventOverlayJobTransforms.overlayTransformUtils import addOverlayBSFilterSubstep, addOverlayBSFilterArguments, addOverlayBSTrigFilterSubstep, addOverlayHITARMakerSubstep
    addOverlayBSTrigFilterSubstep(executorSet)
    addOverlayBSFilterSubstep(executorSet)
    addOverlayHITARMakerSubstep(executorSet)
    trf = transform(executor = executorSet, description = 'Filter BS data based on trigger bit')
    addAthenaArguments(trf.parser)
    addOverlayBSFilterArguments(trf.parser)
    return trf


if __name__ == '__main__':
    main()
