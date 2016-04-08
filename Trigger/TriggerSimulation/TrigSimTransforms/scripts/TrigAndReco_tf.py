#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## TrigAndReco_tf.py - Temporary transform to develop the .
# @version $Id: TrigAndReco_tf.py$ 

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
from RecJobTransforms.recTransformUtils import addAllRecoArgs, addRecoSubsteps

import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigSimTransforms.trfTrigSimUtils import addTrigSimSubsteps, addTrigSimArguments

ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--AddCaloDigi', '--DBRelease', '--DataRunNumber', '--FwdRegionOn', '--LucidOn', '--ZDCOn', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--autoConfiguration', '--beamType', '--bunchSpacing', '--checkEventCount', '--command', '--conditionsTag', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--doAllNoise', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--geometryVersion', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputAODFile', '--inputBSFile', '--inputBeamGasHitsFile', '--inputBeamHaloHitsFile', '--inputCavernHitsFile', '--inputESDFile', '--inputEVNTFile', '--inputHITSFile', '--inputHighPtMinbiasHitsFile', '--inputLowPtMinbiasHitsFile', '--inputNTUP_COMMONFile', '--inputRDOFile', '--jobNumber', '--maxEvents', '--noimf', '--notcmalloc', '--numberOfBeamGas', '--numberOfBeamHalo', '--numberOfCavernBkg', '--numberOfHighPtMinBias', '--numberOfLowPtMinBias', '--outputAODFile', '--outputAOD_SKIMFile', '--outputD2AODM_WMUNUFile', '--outputD2AODM_ZEEFile', '--outputD2AODM_ZMUMUFile', '--outputD2AOD_DIONIAFile', '--outputD2AOD_DIPHOFile', '--outputD2AOD_H4LFile', '--outputD2AOD_WENUFile', '--outputD2AOD_ZEEFile', '--outputD2ESDM_TRKJETFile', '--outputD2ESD_DIPHOFile', '--outputD2ESD_JPSIUPSMMFile', '--outputD2ESD_WENUFile', '--outputD2ESD_WMUNUFile', '--outputD2ESD_ZMMFile', '--outputDAODM_HIGHMJJFile', '--outputDAODM_SGLEMFile', '--outputDAODM_SGLMUFile', '--outputDAODM_SGLPHFile', '--outputDAODM_SGLTAUFile', '--outputDAOD_2EMFile', '--outputDAOD_2L2QHSG2File', '--outputDAOD_2LHSG2File', '--outputDAOD_2MUFile', '--outputDAOD_2PHFile', '--outputDAOD_4LHSG2File', '--outputDAOD_EGLOOSEFile', '--outputDAOD_ELLOOSE18File', '--outputDAOD_ELLOOSEFile', '--outputDAOD_EMJETFile', '--outputDAOD_EMMETFile', '--outputDAOD_EMMUFile', '--outputDAOD_EMTAUFile', '--outputDAOD_H4LBREMRECFile', '--outputDAOD_HIGHMDIJETFile', '--outputDAOD_HSG1File', '--outputDAOD_HSG2File', '--outputDAOD_HWWFile', '--outputDAOD_JETMETFile', '--outputDAOD_JPSIMUMUFile', '--outputDAOD_MUFile', '--outputDAOD_MUJETFile', '--outputDAOD_MUMETFile', '--outputDAOD_MUTAUFile', '--outputDAOD_ONIAMUMUFile', '--outputDAOD_PHLOOSEFile', '--outputDAOD_REDFile', '--outputDAOD_RNDMFile', '--outputDAOD_SKIMELFile', '--outputDAOD_SUSYEGAMMAFile', '--outputDAOD_SUSYJETSFile', '--outputDAOD_SUSYMUONSFile', '--outputDAOD_TAUJETFile', '--outputDAOD_TESTFile', '--outputDAOD_UPSIMUMUFile', '--outputDAOD_ZEEGAMMAFile', '--outputDAOD_ZMUMUGAMMAFile', '--outputDESDM_ALLCELLSFile', '--outputDESDM_BEAMSPOTFile', '--outputDESDM_EGAMMAFile', '--outputDESDM_METFile', '--outputDESDM_MUONFile', '--outputDESDM_RPVLLCCFile', '--outputDESDM_RPVLLFile', '--outputDESDM_TRACKFile', '--outputDESD_CALJETFile', '--outputDESD_CALOCOMMFile', '--outputDESD_COLLCANDFile', '--outputDESD_CTLFWJETFile', '--outputDESD_EGTAUCOMMFile', '--outputDESD_EMCLUSCOMMFile', '--outputDESD_FWJETFile', '--outputDESD_HIRAREFile', '--outputDESD_IDCOMMFile', '--outputDESD_IDPROJCOMMFile', '--outputDESD_MBIASFile', '--outputDESD_MUONCOMMFile', '--outputDESD_ONIAMUMUHIFile', '--outputDESD_PHOJETFile', '--outputDESD_PIXELCOMMFile', '--outputDESD_PRESCALEDFile', '--outputDESD_RANDOMCOMMFile', '--outputDESD_RPCCOMMFile', '--outputDESD_SCTCOMMFile', '--outputDESD_SGLELFile', '--outputDESD_SGLMUFile', '--outputDESD_SKIMELFile', '--outputDESD_TGCCOMMFile', '--outputDESD_TILECOMMFile', '--outputDRAW_IDPROJCOMMFile', '--outputDRAW_WENUFile', '--outputDRAW_WMUNUFile', '--outputDRAW_ZEEFile', '--outputDRAW_ZMUMUFile', '--outputESDFile', '--outputHISTFile', '--outputHIST_AODFile', '--outputNTUPREDFile', '--outputNTUP_2L2QHSG2File', '--outputNTUP_2LHSG2File', '--outputNTUP_4LHSG2File', '--outputNTUP_BKGDFile', '--outputNTUP_BTAGD3PDFile', '--outputNTUP_BTAGEFFFile', '--outputNTUP_BTAGFULLFile', '--outputNTUP_BTAGGHOSTFile', '--outputNTUP_BTAGSLIMFile', '--outputNTUP_CLUSTERCORRECTIONFile', '--outputNTUP_COMMONFile', '--outputNTUP_EGAMMAFile', '--outputNTUP_EMBHHDNFile', '--outputNTUP_EMBHHIMFile', '--outputNTUP_EMBHHUPFile', '--outputNTUP_EMBLHDNFile', '--outputNTUP_EMBLHIMFile', '--outputNTUP_EMBLHUPFile', '--outputNTUP_EMBLLDNFile', '--outputNTUP_EMBLLIMFile', '--outputNTUP_EMBLLUPFile', '--outputNTUP_ENHBIASFile', '--outputNTUP_FASTMONFile', '--outputNTUP_HECNOISEFile', '--outputNTUP_HIFile', '--outputNTUP_HIGHMULTFile', '--outputNTUP_HSG2File', '--outputNTUP_HSG5GAMHFile', '--outputNTUP_HSG5GAMZFile', '--outputNTUP_HSG5WHFile', '--outputNTUP_HSG5WHQFile', '--outputNTUP_HSG5WHUFile', '--outputNTUP_HSG5ZBBFile', '--outputNTUP_HSG5ZHLLFile', '--outputNTUP_HSG5ZHLLUFile', '--outputNTUP_HSG5ZHMETFile', '--outputNTUP_HSG5ZHMETUFile', '--outputNTUP_IDVTXFile', '--outputNTUP_IDVTXLUMIFile', '--outputNTUP_JETMETEMCLFile', '--outputNTUP_JETMETFULLFile', '--outputNTUP_JETMETFile', '--outputNTUP_JETMETWZFile', '--outputNTUP_L1CALOFile', '--outputNTUP_L1CALOPROBFile', '--outputNTUP_L1TGCFile', '--outputNTUP_LARNOISEFile', '--outputNTUP_MCPFile', '--outputNTUP_MINBIASFile', '--outputNTUP_MUFASTFile', '--outputNTUP_MUONCALIBFile', '--outputNTUP_MUONFile', '--outputNTUP_PHOTONFile', '--outputNTUP_PHYSICSFile', '--outputNTUP_PROMPTPHOTFile', '--outputNTUP_SCTFile', '--outputNTUP_SMBKGEFile', '--outputNTUP_SMBKGMUFile', '--outputNTUP_SMDILEPFile', '--outputNTUP_SMDYEEFile', '--outputNTUP_SMDYMUMUFile', '--outputNTUP_SMLIGHTFile', '--outputNTUP_SMQCDFile', '--outputNTUP_SMTRILEPFile', '--outputNTUP_SMWENUFile', '--outputNTUP_SMWENUJJFile', '--outputNTUP_SMWMUNUFile', '--outputNTUP_SMWMUNUJJFile', '--outputNTUP_SMWZFile', '--outputNTUP_SMWZSOFTFile', '--outputNTUP_SMZEEFile', '--outputNTUP_SMZMUMUFile', '--outputNTUP_SUSY01LEPFile', '--outputNTUP_SUSY23LEPFile', '--outputNTUP_SUSY34LEPFile', '--outputNTUP_SUSYBOOSTFile', '--outputNTUP_SUSYFile', '--outputNTUP_SUSYLEPTAUFile', '--outputNTUP_SUSYLLPFile', '--outputNTUP_SUSYRAZORFile', '--outputNTUP_SUSYSKIMFile', '--outputNTUP_SUSYTAGFile', '--outputNTUP_SUSYTRUTHFile', '--outputNTUP_TAUFULLFile', '--outputNTUP_TAUFile', '--outputNTUP_TAUMEDIUMFile', '--outputNTUP_TAUSMALLFile', '--outputNTUP_TOPBOOSTFile', '--outputNTUP_TOPEJETFile', '--outputNTUP_TOPELFile', '--outputNTUP_TOPFile', '--outputNTUP_TOPJETFile', '--outputNTUP_TOPMUFile', '--outputNTUP_TPHSG2File', '--outputNTUP_TRIGBJETFile', '--outputNTUP_TRIGFile', '--outputNTUP_TRIGMUFile', '--outputNTUP_TRKVALIDFile', '--outputNTUP_TRTFile', '--outputNTUP_TRUTHFile', '--outputNTUP_WPRIMEENFile', '--outputNTUP_WPRIMEMNFile', '--outputNTUP_WZFile', '--outputNTUP_ZPRIMEEEFile', '--outputNTUP_ZPRIMEMMFile', '--outputRDOFile', '--outputTAGFile', '--pileupFinalBunch', '--pileupInitialBunch', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reductionConf', '--reportName', '--runNumber', '--samplingFractionDbTag', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--testPileUpConfig', '--tmpRDO', '--triggerConfig']

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
    addRecoSubsteps(executorSet)
    addTrigSimSubsteps(executorSet)
    
    trf = transform(executor = executorSet, description = 'General purpose ATLAS reconstruction transform, which also supports'
                    ' digitisation. Inputs can be HITS, RDO, BS, ESD or AOD, with outputs of RDO, ESD, AOD or DPDs.'
                    ' See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/RecoTf for more details.')
    
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser)
    addTrigSimArguments(trf.parser)
    addAllRecoArgs(trf)

    return trf


if __name__ == '__main__':
    main()
