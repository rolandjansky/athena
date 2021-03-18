# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @brief Module with PAT transform options and substeps

from builtins import zip

# Get the base logger for the transforms and extend it for us
from PyJobTransforms.trfLogger import msg
msg = msg.getChild(__name__)

import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import listKnownD3PDs, getExtraDPDList
from PyJobTransforms.trfExe import  NTUPMergeExecutor, hybridPOOLMergeExecutor

def addPhysValidationFiles(parser):
    # Better to somehow auto-import this from PhysicsAnalysis/PhysicsValidation/PhysValMonitoring
    parser.defineArgGroup('Validation Files', 'Physics validation file options')
    parser.add_argument('--outputNTUP_PHYSVALFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output'),
                        help='Output physics validation file', group='Validation Files')
    
def addPhysValidationMergeFiles(parser):
    # Better to somehow auto-import this from PhysicsAnalysis/PhysicsValidation/PhysValMonitoring
    parser.defineArgGroup('Validation Files', 'Physics validation file options')
    parser.add_argument('--inputNTUP_PHYSVALFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='input'),
                        help='Input physics validation file', group='Validation Files', nargs='+')
    parser.add_argument('--outputNTUP_PHYSVAL_MRGFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output'),
                        help='Output merged physics validation file', group='Validation Files')
    
def addValidationArguments(parser):
    parser.defineArgGroup('Validation Args', 'Physics validation options')
    parser.add_argument('--validationFlags', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argList),
                        help='Physics validation histogram switches', group='Validation Args')
    
def addPhysValidationSubstep(executorSet):
    executorSet.add(athenaExecutor(name = 'PhysicsValidation', skeletonFile = 'PATJobTransforms/skeleton.PhysicsValidation_tf.py',
                                   inData = ['AOD'], outData = ['NTUP_PHYSVAL'], substep='pval'))

def appendPhysValidationSubstep(trf):
    executor = set()
    addPhysValidationSubstep(executor)
    trf.appendToExecutorSet(executor)

def addNTUPMergeSubsteps(executorSet):
    # Ye olde NTUPs
    try:
        # 'Standard' D3PDs
        inDataList, outDataList = listKnownD3PDs()
        for (inData, outData) in iter(zip(inDataList, outDataList)):
            executorSet.add(NTUPMergeExecutor(name='NTUPLEMerge'+inData.replace('_',''), exe='hadd', inData=[inData], outData=[outData], exeArgs=[]))
        # Physics Validation NTUP
        executorSet.add(NTUPMergeExecutor(name='NTUPLEMergePHYSVAL', exe='hadd', inData=['NTUP_PHYSVAL'], outData=['NTUP_PHYSVAL_MRG0'], exeArgs=[]))
        #add post processing for some Pull histograms in ID track monitoring
        executorSet.add(NTUPMergeExecutor(name='NTUPLEMergePHYSVALPostProc', exe='postProcessIDPVMHistos', inData=['NTUP_PHYSVAL_MRG0'], outData=['NTUP_PHYSVAL_MRG'], exeArgs=[]))
        # Extra Tier-0 NTUPs
        extraNTUPs = getExtraDPDList(NTUPOnly = True)
        for ntup in extraNTUPs:
            executorSet.add(NTUPMergeExecutor(name='NTUPLEMerge'+ntup.name.replace('_',''), exe='hadd', inData=[ntup.name], outData=[ntup.name+'_MRG'], exeArgs=[]))
    except ImportError as e:
        msg.warning("Failed to get D3PD lists - probably D3PDs are broken in this release: {0}".format(e))


## @brief Import list of known DAODs from the derivation framework and 
def addDAODArguments(parser, mergerTrf=True):
    DAODTypes = knownDAODTypes()
    if mergerTrf:
        parser.defineArgGroup('Input DAOD', 'Input DAOD files to be merged')
        parser.defineArgGroup('Output DAOD', 'Output merged DAOD files')
        for DAOD in DAODTypes:
            parser.add_argument("--input" + DAOD + "File", nargs="+",
                                type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io="input", type="AOD", subtype=DAOD),
                                help="Input DAOD file of " + DAOD + " derivation", group="Input DAOD")
            parser.add_argument("--output" + DAOD + "_MRGFile", 
                                type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io="output", type="AOD", subtype=DAOD),
                                help="Output merged DAOD file of " + DAOD + " derivation", group="Output DAOD")
    else:
        parser.defineArgGroup('Output DAOD', 'Output derivation DAOD files')
        for DAOD in DAODTypes:
            parser.add_argument("--output" + DAOD + "File", 
                                type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io="output", type="AOD", subtype=DAOD),
                                help="Output DAOD file of " + DAOD + " derivation", group="Output DAOD")


def addDAODMergerSubsteps(executorSet):
    DAODTypes = knownDAODTypes()
    for DAOD in DAODTypes:
        executorSet.add(hybridPOOLMergeExecutor(name = DAOD.lstrip("DAOD_") + 'Merge', skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                        inData = [DAOD], outData = [DAOD+'_MRG'])
                        )

def knownDAODTypes():
    DAODTypes = []
    try:
        from DerivationFrameworkCore.DerivationFrameworkProdFlags import listAODtoDPD
        DAODTypes = [ name.lstrip("Stream") for name in listAODtoDPD ]
    except ImportError:
        msg.warning("Could not import DAOD subtypes from DerivationFramework.DerivationFrameworkCore")
    return DAODTypes
