#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Run ISF simulation on generated events and produce a HITS file.
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
from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addCommonSimTrfArgs, addCommonSimDigTrfArgs, addSim_tfArgs, addHITSMergeArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

# Prodsys hack...
ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--AMIConfig', '--AMITag', '--CosmicFilterVolume', '--CosmicFilterVolume2', '--CosmicPtSlice', '--DBRelease', '--DataRunNumber', '--FwdRegionOn', '--HGTDOn', '--LucidOn', '--ZDCOn', '--argJSON', '--asetup', '--athena', '--athenaMPEventsBeforeFork', '--athenaMPMergeTargetSize', '--athenaMPStrategy', '--athenaMPUseEventOrders', '--athenaopts', '--attempt', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--dumpJSON', '--dumpPickle', '--enableLooperKiller', '--env', '--eventAcceptanceEfficiency', '--eventService', '--execOnly', '--fileValidation', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputEVNT_TRFile', '--inputFileValidation', '--inputHITSFile', '--inputLogsFile', '--jobNumber', '--jobid', '--maxEvents', '--outputEVNT_TRFile', '--outputFileValidation', '--outputHITS_MRGFile', '--outputHITS_RSMFile', '--parallelFileValidation', '--physicsList', '--postExec', '--postInclude', '--preExec', '--preInclude', '--randomSeed', '--reportName', '--reportType', '--runNumber', '--sharedWriter', '--showGraph', '--showPath', '--showSteps', '--simulator', '--skipEvents', '--steering', '--taskid', '--tcmalloc', '--trackRecordType', '--truthStrategy', '--useISF', '--valgrind', '--valgrindDefaultOpts', '--valgrindExtraOpts']

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
    from SimuJobTransforms.SimTransformUtils import addReSimulationSubstep, addReSimulationArguments
    addReSimulationSubstep(executorSet)
    trf = transform(executor = executorSet, description = 'ATLAS Simulation transform. Inputs must be HITS. Outputs must be HITS_RSM')
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addReSimulationArguments(trf.parser)
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
