#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FullChain_tf.py 
#  One step transform that will start from EVNT and run ISF right through 
#  to AOD output
#  @version $Id: AFII_tf.py 568684 2013-11-05 13:03:06Z graemes $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, DQMergeExecutor, reductionFrameworkExecutor, reductionFrameworkExecutorNTUP
from PyJobTransforms.trfArgs import addAthenaArguments, addD3PDArguments, addPrimaryDPDArguments, addExtraDPDTypes, addDetectorArguments, addReductionArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTrfArgs import addCommonRecTrfArgs, addStandardRecoFiles
from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addCosmicsTrfArgs, addForwardDetTrfArgs, addCommonSimTrfArgs, addBasicDigiArgs, addPileUpTrfArgs, addCommonSimDigTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

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

    executorSet.add(athenaExecutor(name = 'Sim_tf', skeletonFile = 'SimuJobTransforms/skeleton.EVGENtoHIT_ISF.py',
                                   substep = 'evnt2hits', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz', inData=['NULL','Evgen'], outData=['HITS','TrackRecord','NULL'] )) #may have to add evgen to outData if cosmics/cavern background jobs don't work.
    executorSet.add(athenaExecutor(name = 'HITtoRDO', skeletonFile = 'SimuJobTransforms/skeleton.HITtoRDO.py',
                                   substep = 'h2r', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz',
                                   inData = ['HITS'], outData = ['RDO'],
                                   runtimeRunargs = {'LowPtMinbiasHitsFile' : 'runArgs.inputLowPtMinbiasHitsFile',
                                                     'HighPtMinbiasHitsFile' : 'runArgs.inputHighPtMinbiasHitsFile',}))
    executorSet.add(athenaExecutor(name = 'RAWtoESD', skeletonFile = 'RecJobTransforms/skeleton.RAWtoESD_tf.py',
                                   substep = 'r2e', inData = ['BS', 'RDO'], outData = ['ESD', 'HIST_ESD_INT'], 
                                   perfMonFile = 'ntuple_RAWtoESD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'ESDtoAOD', skeletonFile = 'RecJobTransforms/skeleton.ESDtoAOD_tf.py',
                                   substep = 'e2a', inData = ['ESD'], outData = ['AOD', 'HIST_AOD_INT'], 
                                   perfMonFile = 'ntuple_ESDtoAOD.pmon.gz'))
    executorSet.add(DQMergeExecutor(name = 'DQHistogramMerge', inData = [('HIST_ESD_INT', 'HIST_AOD_INT')], outData = ['HIST']))
    executorSet.add(athenaExecutor(name = 'ESDtoDPD', skeletonFile = 'PATJobTransforms/skeleton.ESDtoDPD_tf.py',
                                   substep = 'e2d', inData = ['ESD'], outData = [],
                                   perfMonFile = 'ntuple_ESDtoDPD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'AODtoDPD', skeletonFile = 'PATJobTransforms/skeleton.AODtoDPD_tf.py',
                                   substep = 'a2d', inData = ['AOD', 'EVNT'], outData = [],
                                   perfMonFile = 'ntuple_AODtoDPD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'AODtoTAG', skeletonFile = 'RecJobTransforms/skeleton.AODtoTAG_tf.py',
                                   inData = ['AOD'], outData = ['TAG'],))
#     executorSet.add(athenaExecutor(name = 'AODtoHIST', skeletonFile = 'RecJobTransforms/skeleton.FROM_PETER.py',
#                                    inData = ['AOD'], outData = ['HIST_AOD'],))
    executorSet.add(reductionFrameworkExecutor(name = 'AODtoRED', skeletonFile = 'PATJobTransforms/skeleton.AODtoRED_tf.py',
                                   substep = 'a2r', inData = ['AOD'], outData = ['DAOD_RED']))
    executorSet.add(reductionFrameworkExecutorNTUP(name = 'NTUPtoRED', skeletonFile = 'PATJobTransforms/skeleton.NTUPtoRED_tf.py',
                                   substep = 'n2n', inData = ['NTUP_COMMON'], outData = ['NTUP_RED']))

    trf = transform(executor = executorSet, description = 'General purpose ATLAS reconstruction transform, which also supports'
                    ' digitisation. Inputs can be HITS, RDO, BS, ESD or AOD, with outputs of RDO, ESD, AOD or DPDs.'
                    ' See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/RecoTf for more details.')

    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addStandardRecoFiles(trf.parser)
    addPrimaryDPDArguments(trf.parser, transform = trf)
    addD3PDArguments(trf.parser, transform = trf)
    addExtraDPDTypes(trf.parser, transform = trf)
    addReductionArguments(trf.parser, transform = trf)
    addCommonSimTrfArgs(trf.parser)
    addCommonSimDigTrfArgs(trf.parser)
    addCosmicsTrfArgs(trf.parser)
    addBasicDigiArgs(trf.parser)
    addForwardDetTrfArgs(trf.parser)
    addPileUpTrfArgs(trf.parser)
    
    
    return trf


if __name__ == '__main__':
    main()
