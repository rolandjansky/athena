# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Module with PAT transform options and substeps

import logging

# Get the base logger for the transforms and extend it for us
from PyJobTransforms.trfLogger import msg
msg = msg.getChild(__name__)

import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import getExtraDPDList
from PyJobTransforms.trfExe import  NTUPMergeExecutor, hybridPOOLMergeExecutor, archiveExecutor

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
        # Physics Validation NTUP
        executorSet.add(NTUPMergeExecutor(name='NTUPLEMergePHYSVAL', exe='hadd', inData=['NTUP_PHYSVAL'], outData=['NTUP_PHYSVAL_MRG'], exeArgs=[]))
        # Extra Tier-0 NTUPs
        extraNTUPs = getExtraDPDList(NTUPOnly = True)
        for ntup in extraNTUPs:
            executorSet.add(NTUPMergeExecutor(name='NTUPLEMerge'+ntup.name.replace('_',''), exe='hadd', inData=[ntup.name], outData=[ntup.name+'_MRG'], exeArgs=[]))
    except ImportError, e:
        msg.warning("Failed to get extra NTUP list: {0}".format(e))


## @brief Import list of known DAODs from the derivation framework and 
def addDAODArguments(parser, mergerTrf=True):
    DAODTypes = knownDAODTypes()
    if mergerTrf:
        parser.defineArgGroup('Input DAOD', 'Input DAOD files to be merged')
        parser.defineArgGroup('Output DAOD', 'Output merged DAOD files')
        parser.defineArgGroup('Input Logs', 'Input Log files to be merged')
        parser.defineArgGroup('Output Archive', 'Output Archive file')        
        parser.add_argument('--inputDataFile','--inputLogFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='input', type='misc'),
                        help='Input log file(s)', group='Input Logs')
        parser.add_argument('--outputArchFile', 
                            type=trfArgClasses.argFactory(trfArgClasses.argFile, io='output', type='misc'),
                            help='Output archive file', group='Output Archive')
        parser.add_argument('--compressionType', group='Output Archive',
                        help='Underlying compression type', choices=['gzip', 'bzip2', 'none'],
                        default='none')
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
    executorSet.add(archiveExecutor(name = 'Archiver',inData = ['Data'], outData = ['Arch'], exe='tar'))

def knownDAODTypes():
    DAODTypes = []
    try:
        from DerivationFrameworkCore.DerivationFrameworkProdFlags import listAODtoDPD
        DAODTypes = [ name.lstrip("Stream") for name in listAODtoDPD ]
    except ImportError:
        msg.warning("Could not import DAOD subtypes from DerivationFramework.DerivationFrameworkCore")
    return DAODTypes
