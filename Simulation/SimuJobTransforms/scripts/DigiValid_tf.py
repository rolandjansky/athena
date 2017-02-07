#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Run RDO file and produce histograms.
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
import PyJobTransforms.trfArgClasses as trfArgClasses
from SimuJobTransforms.simTrfArgs import addRDOValidArgs

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
    from SimuJobTransforms.SimTransformUtils import addDigiValidationSubstep, addRDOValidArguments
    addDigiValidationSubstep(executorSet)
    trf = transform(executor = executorSet, description = 'ATLAS Validation transform. Inputs must be RDO. Outputs must be histogram files.')
    addAthenaArguments(trf.parser)
    addRDOValidArguments(trf.parser)
    return trf


if __name__ == '__main__':
    main()
