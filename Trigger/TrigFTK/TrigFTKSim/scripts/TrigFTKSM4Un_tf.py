#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Transform
#  Specialist version to do sim x 4 subregions and merge in one job
# @version $Id: TrigFTKSM4Un_tf.py 690574 2015-08-21 11:00:52Z jahreda $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

subregions = 4

from TrigFTKSim.FTKSimOptions import *

ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--CachePath', '--CachedBank', '--ConstantsDir', '--DBBankLevel', '--DCMatchMethod', '--DoRoadFile', '--DuplicateGanged', '--FTKDoGrid', '--FTKForceAllInput', '--FTKSetupTag', '--FTKUnmergedInputPath', '--FitConstantsVersion', '--GangedPatternReco', '--HWModeSS', '--HWNDiff', '--HitWarrior', '--IBLMode', '--MakeCache', '--MaxNcomb', '--MaxNhitsPerPlane', '--MergeRoads', '--MergeRoadsDetailed', '--NBanks', '--NSubRegions', '--PatternsVersion', '--PixelClusteringMode', '--RoadFilesDir', '--SSFAllowExtraMiss', '--SSFMultiConnection', '--SSFNConnections', '--SSFTRDefn', '--SSFTRMaxEta', '--SSFTRMinEta', '--Save1stStageTrks', '--SaveRoads', '--SaveTruthTree', '--SctClustering', '--SecondStageFit', '--SectorsAsPatterns', '--SetAMSize', '--TRACKFITTER_MODE', '--TSPMinCoverage', '--TSPSimulationLevel', '--UseTSPBank', '--argJSON', '--attempt', '--badmap_path', '--badmap_path_for_hit', '--bankpatterns', '--bankregion', '--checkEventCount', '--doAuxFW', '--dumpJSON', '--dumpPickle', '--env', '--execOnly', '--fileValidation', '--firstEvent', '--firstEventFTK', '--skipEvents', '--fit711constantspath', '--fitconstantspath', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputFileValidation', '--inputNTUP_FTKIPFile', '--inputNTUP_FTKTMP_0File', '--inputNTUP_FTKTMP_1File', '--inputNTUP_FTKTMP_2File', '--inputNTUP_FTKTMP_3File', '--inputTXT_FTKIPFile', '--jobid', '--loadHWConf_path', '--maxEvents', '--orphanKiller', '--outputFileValidation', '--outputNTUP_FTKTMPFile', '--outputNTUP_FTKTMP_0File', '--outputNTUP_FTKTMP_1File', '--outputNTUP_FTKTMP_2File', '--outputNTUP_FTKTMP_3File', '--parallelFileValidation', '--patternbank0path', '--patternbank1path', '--patternbank2path', '--patternbank3path', '--patternbankspath', '--pmap_path', '--pmapcomplete_path', '--pmapunused_path', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--rmap_path', '--runNum', '--sectorpath', '--sectorspath', '--separateSubRegFitConst', '--showGraph', '--showPath', '--showSteps', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--ssmap_path', '--ssmaptsp_path', '--ssmapunused_path', '--steering', '--taskid', '--tcmalloc', '--useDBPath', '--versionTag']

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
    for subregion in range(subregions):
        executorSet.add(athenaExecutor(name = 'FTKFullSimulationBank{0}'.format(subregion), 
                                       skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneSim.py',
                                       inData = ['NTUP_FTKIP','TXT_FTKIP'], outData = ['NTUP_FTKTMP_{0}'.format(subregion)],
                                       extraRunargs = {'banksubregion': [subregion]},
                                       # Need to ensure that the correct subregion is used
                                       runtimeRunargs = {'patternbankpath': 'runArgs.patternbank{0}path'.format(subregion),
                                                         'outputNTUP_FTKTMPFile': 'runArgs.outputNTUP_FTKTMP_{0}File'.format(subregion)}))
    executorSet.add(athenaExecutor(name = 'FTKSimulationMerge', 
                                   skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneMerge.py',
                                   inData = [tuple([ 'NTUP_FTKTMP_{0}'.format(subregion) for subregion in range(subregions) ])+('NTUP_FTKIP',)],
                                   outData = ['NTUP_FTKTMP'],
                                   extraRunargs = {'inputNTUP_FTKTMPFile': [ 'tmp.NTUP_FTKTMP_{0}'.format(subregion) for subregion in range(subregions)]},
                                   runtimeRunargs = {'MergeRegion': 'runArgs.bankregion[0]',
                                                     'FirstRegion': 'runArgs.bankregion[0]',
													 'TruthTrackTreeName' : "'truthtracks'",
													 'EvtInfoTreeName' : "'evtinfo'",
                                                     'SaveTruthTree' : '1'
													 },
                                   ))
    trf = transform(executor = executorSet, description = 'FTK Subregion simulate x {0} and merge.'.format(subregions))
 
    addFTKSimulationArgs(trf.parser)
    addTrigFTKSimOptions(trf.parser,nsubregions=subregions)
    addTrigFTKSimMergeOptions(trf.parser);
    addTrigFTKSimTFOptions(trf.parser)
    addTrigFTKSimRFOptions(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation options')

    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                            help='Bank region number', group='TrigFTKSim', nargs='+')
    parser.add_argument('--sectorpath', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                            help='sectors path file for all the subregions', group='TrigFTKSim', nargs='+')

    # Add named parameters for each subregion
    for subregion in range(subregions):
        parser.add_argument('--patternbank{0}path'.format(subregion), type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True), 
                            help='Pattern bank path file, subregion {0}'.format(subregion), group='TrigFTKSim', nargs='+')

    
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')

    # File handling
    parser.add_argument('--inputNTUP_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptmp', treeNames='ftkhits'),
                        help='FTK RDO file in ROOT format'.format(subregion), group='TrigFTKMerge', nargs='+')
    parser.add_argument('--inputTXT_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, runarg=True, io='input', type='txt_ftkip'), 
                        help='Wrapper files (in .dat.bz2 format)', group='TrigFTKSim', nargs='+')
    parser.add_argument('--outputNTUP_FTKTMPFile', '--outputNTUP_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='Subregion merged FTK file'.format(subregion), group='TrigFTKMerge',nargs='+')
    
    # The following for testing only
    for subregion in range(subregions):
        parser.add_argument('--inputNTUP_FTKTMP_{0}File'.format(subregion), 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptmp', treeNames='ftkdata'),
                        help='FTK NTUP file from subregion {0} (for testing only)'.format(subregion), group='TrigFTKSim', nargs='+')
        parser.add_argument('--outputNTUP_FTKTMP_{0}File'.format(subregion), 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftkiptmp', treeNames='ftkdata'),
                        help='FTK NTUP file from subregion {0} (for testing only)'.format(subregion), group='TrigFTKSim', nargs='+')
      
        
    

if __name__ == '__main__':
    main()
