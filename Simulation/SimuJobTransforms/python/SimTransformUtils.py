# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Module with Digitization transform options and substep

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfExe import athenaExecutor

### Add Argument Methods
def addCommonSimDigArguments(parser):
    from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addCommonSimDigTrfArgs
    addCommonSimDigTrfArgs(parser)
    addForwardDetTrfArgs(parser)

def addCommonSimulationArguments(parser):
    from SimuJobTransforms.simTrfArgs import addCommonSimTrfArgs, addCosmicsTrfArgs, addTrackRecordArgs
    addCommonSimTrfArgs(parser)
    addCosmicsTrfArgs(parser)
    addTrackRecordArgs(parser)

def addPureSimulationArguments(parser):
    addCommonSimulationArguments(parser)
    from SimuJobTransforms.simTrfArgs import addSim_tfArgs
    addSim_tfArgs(parser)

def addPureDigitizationArguments(parser):
    from SimuJobTransforms.simTrfArgs import addBasicDigiArgs, addPileUpTrfArgs
    addBasicDigiArgs(parser)
    addPileUpTrfArgs(parser)

def addSimulationArguments(parser):
    addCommonSimDigArguments(parser)
    addPureSimulationArguments(parser)

def addAtlasG4Arguments(parser):
    addCommonSimDigArguments(parser)
    addCommonSimulationArguments(parser)

def addHITSMergeArguments(parser):
    from SimuJobTransforms.simTrfArgs import addHITSMergeArgs
    addHITSMergeArgs(parser)

def addDigitizationArguments(parser):
    addCommonSimDigArguments(parser)
    addPureDigitizationArguments(parser)

def addHITSValidArguments(parser):
    from SimuJobTransforms.simTrfArgs import addHITSValidArgs
    addHITSValidArgs(parser)

### Add Sub-step Methods
## @brief Add ISF transform substep
#  @param overlayTransform If @c True use the tweaked version of in/outData for an overlay job
def addSimulationSubstep(executorSet, overlayTransform = False):
    SimExe = athenaExecutor(name = 'EVNTtoHITS', skeletonFile = 'SimuJobTransforms/skeleton.EVGENtoHIT_ISF.py',
                                   substep = 'sim', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz',
                                   inData=['NULL','EVNT','EVNT_CAVERN','EVNT_COSMICS'],
                                   outData=['EVNT_CAVERNTR','EVNT_COSMICSTR','HITS','NULL'] )
    if overlayTransform:
        from PyJobTransforms.trfUtils import releaseIsOlderThan
        if releaseIsOlderThan(20,3):
            SimExe.inData = [('EVNT', 'BS_SKIM')]
        else:
            SimExe.inData = [('EVNT','TXT_EVENTID')]
        SimExe.outData = ['HITS']
        SimExe.inputDataTypeCountCheck = ['EVNT']
    executorSet.add(SimExe)

def addAtlasG4Substep(executorSet):
    executorSet.add(athenaExecutor(name = 'AtlasG4Tf', skeletonFile = 'SimuJobTransforms/skeleton.EVGENtoHIT_MC12.py',
                                   substep = 'sim', tryDropAndReload = False, 
                                   inData=['NULL','EVNT','EVNT_CAVERN','EVNT_COSMICS'],
                                   outData=['EVNT_CAVERNTR','EVNT_COSMICSTR','HITS','NULL'] ))

def addConfigurableSimSubstep(executorSet, confName, extraSkeleton, confSubStep, confInData, confOutData, confExtraRunargs, confRuntimeRunargs):
    executorSet.add(athenaExecutor(name = confName, skeletonFile = extraSkeleton + ['SimuJobTransforms/skeleton.EVGENtoHIT_MC12.py'],
                                   substep = confSubStep, tryDropAndReload = False,
                                   inData = confInData,
                                   outData = confOutData, extraRunargs = confExtraRunargs, runtimeRunargs = confRuntimeRunargs ))

def addStandardHITSMergeSubstep(executorSet):
    executorSet.add(athenaExecutor(name = 'HITSMerge', substep="hitsmerge", skeletonFile = 'SimuJobTransforms/skeleton.HITSMerge.py',
                                              tryDropAndReload = False, inputDataTypeCountCheck = ['HITS']))

def addAFII_HITSMergeSubstep(executorSet):
    executorSet.add(athenaExecutor(name = 'HITSMerge', substep="hitsmerge", skeletonFile = 'SimuJobTransforms/skeleton.HITSMerge.py',
                                   tryDropAndReload = False,
                                   extraRunargs = {'preInclude': ['FastSimulationJobTransforms/jobConfig.v14_Parametrisation.py','FastCaloSimHit/preInclude.AF2Hit.py'],
                                                   'postInclude': ['FastCaloSimHit/postInclude.AF2FilterHitItems.py','FastSimulationJobTransforms/jobConfig.FastCaloSim_ID_cuts.py','FastSimulationJobTransforms/jobConfig.egamma_lateral_shape_tuning.config20.py']} ))

def addDigitizationSubstep(executorSet):
    executorSet.add(athenaExecutor(name = 'HITtoRDO', skeletonFile = 'SimuJobTransforms/skeleton.HITtoRDO.py',
                                              substep = 'h2r', tryDropAndReload = False,
                                              inData = ['HITS'], outData = ['RDO','RDO_FILT'], runtimeRunargs =
                                              {'LowPtMinbiasHitsFile' : 'runArgs.inputLowPtMinbiasHitsFile',
                                               'HighPtMinbiasHitsFile' : 'runArgs.inputHighPtMinbiasHitsFile',
                                               'cavernHitsFile' : 'runArgs.inputCavernHitsFile',
                                               'beamHaloHitsFile' : 'runArgs.inputBeamHaloHitsFile',
                                               'beamGasHitsFile' : 'runArgs.inputBeamGasHitsFile',}))

def addSimValidationSubstep(executorSet):
    executorSet.add(athenaExecutor(name = 'SimValidation',
                                           skeletonFile = 'SimuJobTransforms/skeleton.HITStoHIST_SIM.py',
                                           inData = ['HITS'], outData = ['HIST_SIM'],))

### Append Sub-step Methods
def appendSimulationSubstep(trf):
    executor = set()
    addSimulationSubstep(executor)
    trf.appendToExecutorSet(executor)

def appendAtlasG4Substep(trf):
    executor = set()
    addAtlasG4Substep(executor)
    trf.appendToExecutorSet(executor)

def appendConfigurableSimSubstep(trf, confName = 'AtlasG4Tf',
                                 extraSkeleton = [], confSubstep = 'sim',
                                 confInData=['NULL','EVNT','EVNT_CAVERN','EVNT_COSMICS'],
                                 confOutData=['EVNT_CAVERNTR','EVNT_COSMICSTR','HITS','NULL'],
                                 confExtraRunargs=None, confRuntimeRunargs=None ):
    executor = set()
    addConfigurableSimSubstep(executor, confName, extraSkeleton, confSubStep, confInData, confOutData, confExtraRunargs, confRuntimeRunargs )
    trf.appendToExecutorSet(executor)

def appendHITSMergeSubstep(trf):
    executor = set()
    addStandardHITSMergeSubstep(executor)
    trf.appendToExecutorSet(executor)

def appendAFII_HITSMergeSubstep(trf):
    executor = set()
    addAFII_HITSMergeSubstep(executor)
    trf.appendToExecutorSet(executor)

def appendDigitizationSubstep(trf):
    executor = set()
    addDigitizationSubstep(executor)
    trf.appendToExecutorSet(executor)

#def appendSimValidationSubstep(trf):
#   executor = set()
#  addSimValidationSubstep(executor)
# trf.appendSimVAlidationSubstep(executor)
