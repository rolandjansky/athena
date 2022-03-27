#! /usr/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments, addTriggerArguments
from RecJobTransforms.recTransformUtils import addRecoSubsteps, addAllRecoArgs
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

# Prodsys hack...
ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--AMIConfig', '--AMITag', '--AddCaloDigi', '--CosmicFilterVolume', '--CosmicFilterVolume2', '--CosmicPtSlice', '--DBRelease', '--DataRunNumber', '--FwdRegionOn', '--LucidOn', '--ReadByteStream', '--ZDCOn', '--argJSON', '--asetup', '--athena', '--athenaMPMergeTargetSize', '--athenaopts', '--attempt', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--digiSteeringConf', '--doAllNoise', '--dumpJSON', '--dumpPickle', '--enableLooperKiller', '--env', '--eventAcceptanceEfficiency', '--outputTXT_EVENTIDFile','--inputTXT_EVENTIDFile', '--execOnly', '--fSampltag', '--fileValidation', '--firstEvent', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputBS_SKIMFile', '--inputEVNTFile', '--inputEVNT_TRFile', '--inputFileValidation', '--inputHITSFile', '--inputZeroBiasBSFile', '--jobNumber', '--jobid', '--lumiBlockMapFile', '--maxEvents', '--maxFilesPerSubjob', '--orphanKiller', '--outputAODFile', '--outputBS_SKIMFile', '--outputESDFile', '--outputEVNT_TRFile', '--outputFileValidation', '--outputHITSFile', '--outputRDOFile', '--outputRDO_FILTFile', '--outputRDO_SGNLFile', '--overlayConfigFile', '--parallelFileValidation', '--physicsList', '--postExec', '--postInclude', '--preExec', '--preInclude', '--randomSeed', '--reportName', '--reportType', '--runNumber', '--samplingFractionDbTag', '--showGraph', '--showPath', '--showSteps', '--simulator', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--taskid', '--tcmalloc', '--tmpRDO', '--tmpRDO_FILT', '--triggerBit', '--truthStrategy', '--useISF', '--valgrind', '--valgrindbasicopts', '--valgrindextraopts', '--WriteRDOFileMetaData']

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():

    msg.info('This is %s', sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    if 'outputFileValidation' in  trf._argdict and  trf._argdict['outputFileValidation'].value is False:
        msg.info('Skipping report generation')
    else:
        trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d", (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
    from EventOverlayJobTransforms.overlayTransformUtils import addOverlayBSFilterSubstep, addOverlay_BSSubstep, addCommonOverlayArguments, addUniqueOverlayBSFilterArguments, addUniqueOverlay_BSArguments, addOverlayChainOverrideArguments
    from SimuJobTransforms.SimTransformUtils import addSimulationSubstep, addSimulationArguments
    addRecoSubsteps(executorSet)
    addOverlayBSFilterSubstep(executorSet)
    addSimulationSubstep(executorSet, overlayTransform = True)
    addOverlay_BSSubstep(executorSet)
    trf = transform(executor = executorSet, description = 'Full Overlay Chain')

    addOverlayChainOverrideArguments(trf.parser)
    addAthenaArguments(trf.parser, maxEventsDefaultSubstep='all')
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser, addTrigFilter=False)
    addCommonOverlayArguments(trf.parser)
    addUniqueOverlayBSFilterArguments(trf.parser)
    addSimulationArguments(trf.parser)
    addUniqueOverlay_BSArguments(trf.parser)

    # Reconstruction arguments and outputs 
    addAllRecoArgs(trf)
   
   
    return trf

if __name__ == '__main__':
    main()
