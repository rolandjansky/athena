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
from SimuJobTransforms.simTrfArgs import addCommonSimTrfArgs, addCommonSimDigTrfArgs, addTestBeamArgs


import PyJobTransforms.trfArgClasses as trfArgClasses

# Prodsys hack...
ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--DBRelease', '--DataRunNumber', '--Eta', '--Theta', '--Z', '--asetup', '--athena', '--athenaopts', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--enableLooperKiller', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--firstEvent', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputEvgenFile', '--jobNumber', '--maxEvents', '--outputHITSFile', '--physicsList', '--postExec', '--postInclude', '--preExec', '--preInclude', '--randomSeed', '--reportName', '--reportType', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--tcmalloc', '--testBeamConfig', '--useISF']

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
    trf = transform(executor = athenaExecutor(name = 'TestBeamTf', skeletonFile = 'SimuJobTransforms/skeleton.TestBeam.py',
                                              substep = 'TBsim', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz', inData=['NULL','Evgen'], outData=['HITS','NULL'] )) #may have to add evgen to outData if cosmics/cavern background jobs don't work.
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonSimTrfArgs(trf.parser)
    addCommonSimDigTrfArgs(trf.parser)
    addTestBeamArgs(trf.parser)
    return trf

## FIXME - not sure what the equivalent of the method below is in the new framework?

##     def doPreRunActions(self):
##         JobTransform.doPreRunActions(self)
##         if hasattr(self,'_maxEventsStrategy'):
##             self._maxEventsStrategy = 'ABORT'
##         else:
##             print "WARNING EVGENtoHITJobTransform has no attribute \'_maxEventsStrategy\'."


if __name__ == '__main__':
    main()
