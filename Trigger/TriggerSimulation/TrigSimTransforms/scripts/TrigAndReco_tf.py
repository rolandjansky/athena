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
