# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Module with PAT transform options and substeps

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import listKnownD3PDs
from PyJobTransforms.trfExe import  NTUPMergeExecutor
#from PyJobTransforms.trfArgs import addD3PDArguments, addPrimaryDPDArguments, addExtraDPDTypes, addReductionArguments

def addPhysValidationFiles(parser):
    # Better to somehow auto-import this from PhysicsAnalysis/PhysicsValidation/PhysValMonitoring
    parser.defineArgGroup('Validation Files', 'Physics validation file options')
    parser.add_argument('--outputNTUP_PHYSVALFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output'),
                        help='Output physics validation file', group='Validation Files')
    
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
    inDataList, outDataList = listKnownD3PDs()
    for (inData, outData) in iter(zip(inDataList, outDataList)):
        executorSet.add(NTUPMergeExecutor(name='NTUPLEMerge'+inData.replace('_',''), exe='hadd', inData=[inData], outData=[outData], exeArgs=[]))
    