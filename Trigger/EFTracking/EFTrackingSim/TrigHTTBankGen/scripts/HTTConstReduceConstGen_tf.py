# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python

## HTT Simulation Transform
# @version $Id: HTTConstReduceConstGen_tf.py 718878 2016-01-20 17:27:51Z jwebster $ 
import argparse
import sys
import time
import traceback

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses
from TrigHTTConfig.parseRunArgs import addHTTMapsArgs

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


## Get the base transform with all arguments added
def getTransform():
    executorSet = set()
    executorSet.add(athenaExecutor(name = 'HTTConstantGen2ndStage', 
                                   skeletonFile = 'TrigHTTBankGen/skeleton.HTTConstantGen.py', 
                                   substep='HTTC2',
                                   inData=['HTTMatrix'], outData=['HTTGoodMatrix']      
                                  ))
    executorSet.add(athenaExecutor(name = 'HTTMatrixReduction', 
                                   skeletonFile = 'TrigHTTBankGen/skeleton.HTTMatrixReduction.py',
                                   inData=['HTTGoodMatrix'], outData=['HTTMatrixReduced'],
                                   substep='HTTM2m'
                                  ))
    executorSet.add(athenaExecutor(name = 'HTTConstantGen1stStageIgnore2ndStage', 
                                   skeletonFile = 'TrigHTTBankGen/skeleton.HTTConstantGen.py',
                                   runtimeRunargs={'CheckGood2ndStage':0},
                                   inData=['HTTMatrixReduced'], outData=['HTTGoodMatrixReducedIgnore'],
                                   substep='HTTC1I'
                                  ))
    executorSet.add(athenaExecutor(name = 'HTTConstantGen1stStageCheck2ndStage', 
                                   skeletonFile = 'TrigHTTBankGen/skeleton.HTTConstantGen.py',
                                   runtimeRunargs={'CheckGood2ndStage':1},
                                   inData=['HTTMatrixReduced'], outData=['HTTGoodMatrixReducedCheck'],
                                   substep='HTTC1C'
                                  ))

    trf = transform(executor=executorSet, description = "HTT Make 2nd stage constants, reduce matrix file, make 1st stage constants")

    addHTTConstReduceGenArgs(trf.parser)
    addHTTMapsArgs(trf.parser)

    return trf


def addHTTConstReduceGenArgs(parser):
    # Add a specific HTT argument group
    parser.defineArgGroup('HTTConstReduceConst', 'Fast tracker simulation options')

    parser.add_argument('--CheckGood2ndStage', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Check whether 2nd stage sector is good when producing things for 1st stage', group='HTTConstReduceConst')

    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Bank region number', group='HTTConstReduceConst')
   
    parser.add_argument('--genconst', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Generate the sectors and constants', group='HTTConstReduceConst')

    parser.add_argument('--allregions', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Run all regions?', group='HTTConstReduceConst')
    
    parser.add_argument('--inputHTTMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='input',type='htt_matrix'),help="input matrix file", group="HTTConstReduceConst")

    parser.add_argument('--inputHTTMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='input',type='htt_matrix'),help="input matrix file", group="HTTConstReduceConst")


    parser.add_argument('--outputHTTGoodMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='output',type='htt_matrix'), help="output good matrix file", group="HTTConstReduceConst")

    parser.add_argument('--inputHTTGoodMatrixFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='input'), 
                       help="input good matrix file", group="HTTConstReduceConst" )

    parser.add_argument('--outputHTTMatrixReducedFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='output',type='htt_matrix_reduced'), help="output reduced matrix file", group="HTTConstReduceConst")

    parser.add_argument('--outputHTTGoodMatrixReducedIgnoreFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='output',type='htt_matrix_good_reduced_ignore'), help="output reduced good matrix file, ignoring whether 2nd stage is good or not", group="HTTConstReduceConst")

    parser.add_argument('--outputHTTGoodMatrixReducedCheckFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='output',type='htt_matrix_good_reduced_check'), help="output reduced good matrix file, check whether 2nd stage is good or not", group="HTTConstReduceConst")

    parser.add_argument('--inputHTTMatrixReducedFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile,runarg=True,io='input',type='htt_matrix_reduced'), help="input reduced matrix file", group="HTTConstReduceConst")

    parser.add_argument('--extract1stStage', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Reduce 2nd stage matrix to 1st stage matrix', group='HTTConstReduceConst')
    

if __name__ == '__main__':
    main()
