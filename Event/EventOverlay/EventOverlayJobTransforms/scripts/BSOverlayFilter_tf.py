#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Transform for preparation of BS overlay
# @version $Id: BSOverlayFilter_tf.py 634748 2014-12-09 14:59:16Z jchapman $ 

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

ListOfDefaultPositionalKeys=['--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--checkEventCount', '--command', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--inputBSFile', '--jobNumber', '--lumiBlockMapFile', '--maxEvents', '--maxFilesPerSubjob', '--noimf', '--notcmalloc', '--outputBS_SKIMFile', '--overlayConfigFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--triggerBit']

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
    from EventOverlayJobTransforms.overlayTransformUtils import addOverlayBSFilterSubstep, addOverlayBSFilterArguments
    addOverlayBSFilterSubstep(executorSet)
    trf = transform(executor = executorSet, description = 'Filter BS data based on trigger bit')
    addAthenaArguments(trf.parser)
    addOverlayBSFilterArguments(trf.parser)
    return trf


if __name__ == '__main__':
    main()
