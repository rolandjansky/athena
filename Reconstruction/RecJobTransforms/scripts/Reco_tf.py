#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## Reco_tf.py - New transform re-implementation of the beast that is Reco_trf.
# @version $Id: Reco_tf.py 769766 2016-08-24 09:49:03Z lerrenst $ 

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments, addTriggerArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addAllRecoArgs, addRecoSubsteps, detectRAWtoALL

ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--AMIConfig', '--AMITag', '--AddCaloDigi', '--CA', '--DBRelease', '--DataRunNumber', '--FwdRegionOn', '--HGTDOn', '--LucidOn', '--PileUpPremixing', '--ZDCOn', '--argJSON', '--asetup', '--athena', '--athenaMPEventsBeforeFork', '--athenaMPMergeTargetSize', '--athenaMPStrategy', '--athenaMPUseEventOrders', '--athenaopts', '--attempt', '--autoConfiguration', '--beamType', '--bunchSpacing', '--checkEventCount', '--command', '--conditionsTag', '--detectors', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--digiSteeringConf', '--doAllNoise', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--eventService', '--execOnly', '--fileValidation', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputAODFile', '--inputBSFile', '--inputBeamGasHitsFile', '--inputBeamHaloHitsFile', '--inputCavernHitsFile', '--inputDRAW_EMUFile', '--inputDRAW_ZEEFile', '--inputDRAW_ZMUMUFile', '--inputESDFile', '--inputEVNTFile', '--inputFileValidation', '--inputHITSFile', '--inputHighPtMinbiasHitsFile', '--inputLowPtMinbiasHitsFile', '--inputNTUP_COMMONFile', '--inputRDOFile', '--inputRDO_BKGFile', '--inputRDO_FTKFile', '--inputRDO_TRIGFile', '--jobNumber', '--jobid', '--maxEvents', '--multiprocess', '--multithreaded', '--numberOfBeamGas', '--numberOfBeamHalo', '--numberOfCavernBkg', '--numberOfHighPtMinBias', '--numberOfLowPtMinBias', '--outputAODFile', '--outputAOD_RPRFile', '--outputAOD_SKIMFile', '--outputBSFile', '--outputDAODFile', '--outputDAOD_HSG2File', '--outputDAOD_IDNCBFile', '--outputDAOD_IDPIXLUMIFile', '--outputDAOD_IDTIDEFile', '--outputDAOD_IDTRKLUMIFile', '--outputDAOD_IDTRKVALIDFile', '--outputDAOD_L1CALO1File', '--outputDAOD_L1CALO2File', '--outputDAOD_L1CALO3File', '--outputDAOD_L1CALO4File', '--outputDAOD_L1CALO5File', '--outputDAOD_PIXELVALIDFile', '--outputDAOD_SCTVALIDFile', '--outputDESDM_ALLCELLSFile', '--outputDESDM_CALJETFile', '--outputDESDM_EGAMMAFile', '--outputDESDM_EOVERPFile', '--outputDESDM_EXOTHIPFile', '--outputDESDM_IDALIGNFile', '--outputDESDM_MCPFile', '--outputDESDM_PHOJETFile', '--outputDESDM_SGLELFile', '--outputDESDM_SLTTMUFile', '--outputDESDM_TILEMUFile', '--outputDESDM_ZMUMUFile', '--outputDESD_DEDXFile', '--outputDNTUPFile', '--outputDRAW_BCID1File', '--outputDRAW_BCID2File', '--outputDRAW_BCID3File', '--outputDRAW_BCID4File', '--outputDRAW_EGJPSIFile', '--outputDRAW_EGZFile', '--outputDRAW_EMUFile', '--outputDRAW_RPVLLFile', '--outputDRAW_TAUMUHFile', '--outputDRAW_TOPSLMUFile', '--outputDRAW_ZMUMUFile', '--outputESDFile', '--outputFileValidation', '--outputHISTFile', '--outputHIST_AODFile', '--outputNTUP_ENHBIASFile', '--outputNTUP_FASTMONFile', '--outputNTUP_FastCaloSimFile', '--outputNTUP_HECNOISEFile', '--outputNTUP_HIGHMULTFile', '--outputNTUP_LARNOISEFile', '--outputNTUP_MCPScaleFile', '--outputNTUP_MCPTPFile', '--outputNTUP_MUONCALIBFile', '--outputNTUP_PHYSVALFile', '--outputNTUP_PILEUPFile', '--outputNTUP_PROMPTPHOTFile', '--outputNTUP_SCTFile', '--outputNTUP_SUSYTRUTHFile', '--outputNTUP_TRKVALIDFile', '--outputNTUP_TRTFile', '--outputNTUP_TRUTHFile', '--outputNTUP_WZFile', '--outputRDOFile', '--outputRDO_FILTFile', '--outputRDO_SGNLFile', '--outputRDO_TRIGFile', '--outputTAGFile', '--outputTXT_JIVEXMLTGZFile', '--parallelFileValidation', '--passThrough', '--pileupFinalBunch', '--pileupInitialBunch', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reductionConf', '--reportName', '--reportType', '--runNumber', '--samplingFractionDbTag', '--sharedWriter', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--steering', '--taskid', '--tcmalloc', '--testPileUpConfig', '--topOptions', '--trigFilterList', '--trigStream', '--triggerConfig', '--valgrind', '--valgrindDefaultOpts', '--valgrindExtraOpts', '--valid', '--validationFlags']

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s', sys.argv[0])

    if "--help" in sys.argv[1:]:
        # just looking for help, skip r2a limited parser
        RAWtoALL = False
    else:
        RAWtoALL = detectRAWtoALL(sys.argv[1:])

    if RAWtoALL:
        msg.info("RAWtoALL workflow detected")
    else:
        msg.info("no RAWtoALL")

    trf = getTransform(RAWtoALL)
    trf.parseCmdLineArgs(sys.argv[1:])

    # Just add a note here that this is the place to insert extra checks or manipulations
    # after the arguments are known, but before the transform tries to trace the graph
    # path or actually execute (e.g., one can add some steering based on defined arguments) 

    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d", sys.argv[0], time.asctime(), trf.exitCode)
    sys.exit(trf.exitCode)

def getTransform(RAWtoALL=False):
    executorSet = set()
    addRecoSubsteps(executorSet)

    trf = transform(executor = executorSet, description = 'General purpose ATLAS digitisation and reconstruction transform'
                    ' Inputs can be HITS, RDO, BS, ESD or AOD, with outputs of RDO, ESD, AOD or DAODs.'
                    ' See https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RecoTf for more details.')
    
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser)
    addAllRecoArgs(trf, RAWtoALL)
    
    # For digi step - make sure we can add the digitisation/simulation arguments
    # before we add this substep; allows Reco_tf to work without AtlasSimulation
    try:
        from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addBasicDigiArgs, addPileUpTrfArgs, addCommonSimDigTrfArgs
        from SimuJobTransforms.SimTransformUtils import addDigitizationSubstep
        addBasicDigiArgs(trf.parser)
        addForwardDetTrfArgs(trf.parser)
        addPileUpTrfArgs(trf.parser)
        addCommonSimDigTrfArgs(trf.parser)
        simStepSet = set()
        addDigitizationSubstep(simStepSet)
        trf.appendToExecutorSet(list(simStepSet)[0])
    except ImportError as e:
        msg.warning('Failed to import digitisation arguments ({0}). Digitisation substep will not be available.'.format(e))
        
    # Again, protect core functionality from too tight a dependence on EventOverlay
    try:
        from OverlayConfiguration.OverlayTransformHelpers import addOverlayArguments, appendOverlaySubstep
        addOverlayArguments(trf.parser, in_reco_chain=True)
        appendOverlaySubstep(trf, in_reco_chain=True)
    except ImportError as e:
        msg.warning('Failed to import overlay arguments ({0}). Overlay substep will not be available.'.format(e))
    
    # Again, protect core functionality from too tight a dependence on PATJobTransforms
    try:
        from PATJobTransforms.PATTransformUtils import addPhysValidationFiles, addValidationArguments, appendPhysValidationSubstep
        addPhysValidationFiles(trf.parser)
        addValidationArguments(trf.parser)
        appendPhysValidationSubstep(trf)
    except ImportError:
        msg.warning('Failed to import PAT arguments. Physics validation substep will not be available.')
        
    return trf


if __name__ == '__main__':
    main()
