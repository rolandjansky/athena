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

ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--DBRelease', '--argJSON', '--asetup', '--athena', '--athenaMPEventsBeforeFork', '--athenaMPMergeTargetSize', '--athenaMPStrategy', '--athenaMPUseEventOrders', '--athenaopts', '--attempt', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--copyJO', '--dumpJSON', '--dumpPickle', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--fileValidation', '--geometryVersion', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputAODFile', '--inputBSFile', '--inputBS_OLDFile', '--inputBS_TRIG_OLDFile', '--inputDRAW_EMUFile', '--inputDRAW_ZEEFile', '--inputDRAW_ZMUMUFile', '--inputESDFile', '--inputEVNTFile', '--inputFileValidation', '--inputNTUP_COMMONFile', '--inputRDOFile', '--inputRDO_FTKFile', '--inputRDO_TRIGFile', '--jobid', '--maxEvents', '--outputAODFile', '--outputAOD_RPRFile', '--outputAOD_SKIMFile', '--outputBSFile', '--outputBS_OLDFile', '--outputBS_TRIG_OLDFile', '--outputDAODFile', '--outputDAOD_HSG2File', '--outputDAOD_IDNCBFile', '--outputDAOD_IDPIXLUMIFile', '--outputDAOD_IDTIDEFile', '--outputDAOD_IDTRKLUMIFile', '--outputDAOD_IDTRKVALIDFile', '--outputDAOD_L1CALO1File', '--outputDAOD_L1CALO2File', '--outputDAOD_L1CALO3File', '--outputDAOD_RPVLLFile', '--outputDAOD_SCTVALIDFile', '--outputDESDM_ALLCELLSFile', '--outputDESDM_CALJETFile', '--outputDESDM_EGAMMAFile', '--outputDESDM_EXOTHIPFile', '--outputDESDM_IDALIGNFile', '--outputDESDM_MCPFile', '--outputDESDM_PHOJETFile', '--outputDESDM_SGLELFile', '--outputDESDM_SLTTMUFile', '--outputDESDM_TILEMUFile', '--outputDESDM_ZMUMUFile', '--outputDESD_DEDXFile', '--outputDNTUPFile', '--outputDRAW_EGJPSIFile', '--outputDRAW_EGZFile', '--outputDRAW_EMUFile', '--outputDRAW_RPVLLFile', '--outputDRAW_TAUMUHFile', '--outputDRAW_ZMUMUFile', '--outputESDFile', '--outputFileValidation', '--outputHISTFile', '--outputHIST_AODFile', '--outputNTUP_ENHBIASFile', '--outputNTUP_FASTMONFile', '--outputNTUP_FastCaloSimFile', '--outputNTUP_HECNOISEFile', '--outputNTUP_HIGHMULTFile', '--outputNTUP_LARNOISEFile', '--outputNTUP_MCPScaleFile', '--outputNTUP_MCPTPFile', '--outputNTUP_MUONCALIBFile', '--outputNTUP_PROMPTPHOTFile', '--outputNTUP_SCTFile', '--outputNTUP_SUSYTRUTHFile', '--outputNTUP_TRKVALIDFile', '--outputNTUP_TRTFile', '--outputNTUP_TRUTHFile', '--outputNTUP_WZFile', '--outputRDO_TRIGFile', '--outputRDO_TRIG_OLDFile', '--outputTAGFile', '--outputTXT_JIVEXMLTGZFile', '--parallelFileValidation', '--passThrough', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reductionConf', '--reportName', '--reportType', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--steering', '--taskid', '--tcmalloc', '--topOptions', '--trigFilterList', '--trigStream', '--triggerConfig', '--valgrind', '--valgrindDefaultOpts', '--valgrindExtraOpts', '--valid']


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
                    ' See https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RecoTf for more details.')
    
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addTriggerArguments(trf.parser)
    addTrigSimArguments(trf.parser)
    addAllRecoArgs(trf)

    return trf


if __name__ == '__main__':
    main()
