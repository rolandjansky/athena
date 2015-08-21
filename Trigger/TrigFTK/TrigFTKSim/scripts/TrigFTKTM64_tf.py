#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Transform
#  Specialist version to do sim x 4 subregions and merge in one job
# @version $Id: TrigFTKSM4Un_tf.py 579246 2014-01-20 17:48:57Z gvolpi $ 

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

from TrigFTKSim.FTKSimOptions import *
 
ntowers = 64
subregions = 1


ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--CachePath', '--CachedBank', '--ConstantsDir', '--DBBankLevel', '--DCMatchMethod', '--DoRoadFile', '--DuplicateGanged', '--FTKDoGrid', '--FTKForceAllInput', '--FTKSetupTag', '--FTKUnmergedInputPath', '--FitConstantsVersion', '--GangedPatternReco', '--HWModeSS', '--HWNDiff', '--HitWarrior', '--IBLMode', '--MakeCache', '--MaxNcomb', '--MaxNhitsPerPlane', '--MergeRoads', '--MergeRoadsDetailed', '--NBanks', '--NSubRegions', '--PatternsVersion', '--PixelClusteringMode', '--RoadFilesDir', '--SSFAllowExtraMiss', '--SSFMultiConnection', '--SSFNConnections', '--SSFTRDefn', '--SSFTRMaxEta', '--SSFTRMinEta', '--Save1stStageTrks', '--SaveRoads', '--SaveTruthTree', '--SctClustering', '--SecondStageFit', '--SectorsAsPatterns', '--SetAMSize', '--TRACKFITTER_MODE', '--TSPMinCoverage', '--TSPSimulationLevel', '--UseTSPBank', '--argJSON', '--attempt', '--badmap_path', '--badmap_path_for_hit', '--bankpatterns', '--checkEventCount', '--doAuxFW', '--dumpJSON', '--dumpPickle', '--env', '--execOnly', '--fileValidation', '--firstEventFTK', '--firstEvent', '--fit711constantspath', '--fitconstantspath', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputFileValidation', '--inputNTUP_FTKIPFile', '--inputNTUP_FTKTMP_0File', '--inputNTUP_FTKTMP_10File', '--inputNTUP_FTKTMP_11File', '--inputNTUP_FTKTMP_12File', '--inputNTUP_FTKTMP_13File', '--inputNTUP_FTKTMP_14File', '--inputNTUP_FTKTMP_15File', '--inputNTUP_FTKTMP_16File', '--inputNTUP_FTKTMP_17File', '--inputNTUP_FTKTMP_18File', '--inputNTUP_FTKTMP_19File', '--inputNTUP_FTKTMP_1File', '--inputNTUP_FTKTMP_20File', '--inputNTUP_FTKTMP_21File', '--inputNTUP_FTKTMP_22File', '--inputNTUP_FTKTMP_23File', '--inputNTUP_FTKTMP_24File', '--inputNTUP_FTKTMP_25File', '--inputNTUP_FTKTMP_26File', '--inputNTUP_FTKTMP_27File', '--inputNTUP_FTKTMP_28File', '--inputNTUP_FTKTMP_29File', '--inputNTUP_FTKTMP_2File', '--inputNTUP_FTKTMP_30File', '--inputNTUP_FTKTMP_31File', '--inputNTUP_FTKTMP_32File', '--inputNTUP_FTKTMP_33File', '--inputNTUP_FTKTMP_34File', '--inputNTUP_FTKTMP_35File', '--inputNTUP_FTKTMP_36File', '--inputNTUP_FTKTMP_37File', '--inputNTUP_FTKTMP_38File', '--inputNTUP_FTKTMP_39File', '--inputNTUP_FTKTMP_3File', '--inputNTUP_FTKTMP_40File', '--inputNTUP_FTKTMP_41File', '--inputNTUP_FTKTMP_42File', '--inputNTUP_FTKTMP_43File', '--inputNTUP_FTKTMP_44File', '--inputNTUP_FTKTMP_45File', '--inputNTUP_FTKTMP_46File', '--inputNTUP_FTKTMP_47File', '--inputNTUP_FTKTMP_48File', '--inputNTUP_FTKTMP_49File', '--inputNTUP_FTKTMP_4File', '--inputNTUP_FTKTMP_50File', '--inputNTUP_FTKTMP_51File', '--inputNTUP_FTKTMP_52File', '--inputNTUP_FTKTMP_53File', '--inputNTUP_FTKTMP_54File', '--inputNTUP_FTKTMP_55File', '--inputNTUP_FTKTMP_56File', '--inputNTUP_FTKTMP_57File', '--inputNTUP_FTKTMP_58File', '--inputNTUP_FTKTMP_59File', '--inputNTUP_FTKTMP_5File', '--inputNTUP_FTKTMP_60File', '--inputNTUP_FTKTMP_61File', '--inputNTUP_FTKTMP_62File', '--inputNTUP_FTKTMP_63File', '--inputNTUP_FTKTMP_6File', '--inputNTUP_FTKTMP_7File', '--inputNTUP_FTKTMP_8File', '--inputNTUP_FTKTMP_9File', '--inputTXT_FTKIPFile', '--jobid', '--loadHWConf_path', '--maxEvents', '--orphanKiller', '--outputFileValidation', '--outputNTUP_FTKFile', '--outputNTUP_FTKTMP_0File', '--outputNTUP_FTKTMP_10File', '--outputNTUP_FTKTMP_11File', '--outputNTUP_FTKTMP_12File', '--outputNTUP_FTKTMP_13File', '--outputNTUP_FTKTMP_14File', '--outputNTUP_FTKTMP_15File', '--outputNTUP_FTKTMP_16File', '--outputNTUP_FTKTMP_17File', '--outputNTUP_FTKTMP_18File', '--outputNTUP_FTKTMP_19File', '--outputNTUP_FTKTMP_1File', '--outputNTUP_FTKTMP_20File', '--outputNTUP_FTKTMP_21File', '--outputNTUP_FTKTMP_22File', '--outputNTUP_FTKTMP_23File', '--outputNTUP_FTKTMP_24File', '--outputNTUP_FTKTMP_25File', '--outputNTUP_FTKTMP_26File', '--outputNTUP_FTKTMP_27File', '--outputNTUP_FTKTMP_28File', '--outputNTUP_FTKTMP_29File', '--outputNTUP_FTKTMP_2File', '--outputNTUP_FTKTMP_30File', '--outputNTUP_FTKTMP_31File', '--outputNTUP_FTKTMP_32File', '--outputNTUP_FTKTMP_33File', '--outputNTUP_FTKTMP_34File', '--outputNTUP_FTKTMP_35File', '--outputNTUP_FTKTMP_36File', '--outputNTUP_FTKTMP_37File', '--outputNTUP_FTKTMP_38File', '--outputNTUP_FTKTMP_39File', '--outputNTUP_FTKTMP_3File', '--outputNTUP_FTKTMP_40File', '--outputNTUP_FTKTMP_41File', '--outputNTUP_FTKTMP_42File', '--outputNTUP_FTKTMP_43File', '--outputNTUP_FTKTMP_44File', '--outputNTUP_FTKTMP_45File', '--outputNTUP_FTKTMP_46File', '--outputNTUP_FTKTMP_47File', '--outputNTUP_FTKTMP_48File', '--outputNTUP_FTKTMP_49File', '--outputNTUP_FTKTMP_4File', '--outputNTUP_FTKTMP_50File', '--outputNTUP_FTKTMP_51File', '--outputNTUP_FTKTMP_52File', '--outputNTUP_FTKTMP_53File', '--outputNTUP_FTKTMP_54File', '--outputNTUP_FTKTMP_55File', '--outputNTUP_FTKTMP_56File', '--outputNTUP_FTKTMP_57File', '--outputNTUP_FTKTMP_58File', '--outputNTUP_FTKTMP_59File', '--outputNTUP_FTKTMP_5File', '--outputNTUP_FTKTMP_60File', '--outputNTUP_FTKTMP_61File', '--outputNTUP_FTKTMP_62File', '--outputNTUP_FTKTMP_63File', '--outputNTUP_FTKTMP_6File', '--outputNTUP_FTKTMP_7File', '--outputNTUP_FTKTMP_8File', '--outputNTUP_FTKTMP_9File', '--parallelFileValidation', '--patternbankspath', '--pmap_path', '--pmapcomplete_path', '--pmapunused_path', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--reportType', '--rmap_path', '--runNum', '--sectorspath', '--separateSubRegFitConst', '--showGraph', '--showPath', '--showSteps', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--ssmap_path', '--ssmaptsp_path', '--ssmapunused_path', '--steering', '--taskid', '--tcmalloc', '--useDBPath', '--versionTag']

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
    for tower in range(ntowers) :
        executorSet.add(athenaExecutor(name = 'FTKFullSimulationBank{0}'.format(tower), 
              skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneSim.py',
              inData = ['NTUP_FTKIP','TXT_FTKIP'], outData = ['NTUP_FTKTMP_{0}'.format(tower)],
              extraRunargs = {'bankregion': [tower],'banksubregion': [0]},
              # Need to ensure that the correct subregion is used
              runtimeRunargs = {'bankregion': [tower], 'subregions': [0],
               'patternbankpath': '[runArgs.patternbankspath[{0}]]'.format(tower),
               'fitconstantspath': '[runArgs.fitconstantspath[{0}]]'.format(tower),
               'fit711constantspath': '[runArgs.fit711constantspath[{0}]]'.format(tower),
               'sectorpath': '[runArgs.sectorspath[{0}]]'.format(tower),
               'outputNTUP_FTKTMPFile': 'runArgs.outputNTUP_FTKTMP_{0}File'.format(tower)}))

    # add final merge for all the tower, generating a NTUP_FTK file
    executorSet.add(athenaExecutor(name = "FTKSimulationMergeFinal",
          skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneMerge.py',
          inData = [tuple([ 'NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers) ])+('NTUP_FTKIP',)],
          outData = ['NTUP_FTK'],
          extraRunargs = {'inputNTUP_FTKTMPFile': [ 'tmp.NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers)]},
          runtimeRunargs = {'MergeFromTowers': False,
                            'FirstRegion': 0,
                            'TruthTrackTreeName' : "'truthtracks'",
                            'EvtInfoTreeName' : "'evtinfo'",
                            'SaveTruthTree' : '1'}))

    trf = transform(executor = executorSet, description = 'FTK simulation for {0} towers, with no subregions, and final merge.'.format(ntowers))

    addFTKSimulationArgs(trf.parser)
    addTrigFTKSimMergeOptions(trf.parser);
    addTrigFTKSimOptions(trf.parser,nsubregions=subregions)
    addTrigFTKSimRFOptions(trf.parser)
    addTrigFTKSimTFOptions(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')
    
    # File handling
    parser.add_argument('--inputNTUP_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkip', treeNames='ftkhits'),
                        help='FTK RDO file in ROOT format', group='TrigFTKSim', nargs='+')
    parser.add_argument('--inputTXT_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, runarg=True, io='input', type='txt_ftkip'), 
                        help='Wrapper files (in .dat.bz2 format)', group='TrigFTKSim', nargs='+')
    
    # The following for testing only
    for tower in range(ntowers) :
      parser.add_argument('--outputNTUP_FTKTMP_{0}File'.format(tower), 
                          type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                          help='Tower {0} merged FTK file (for test only)'.format(tower), group='TrigFTKMerge')
      parser.add_argument('--inputNTUP_FTKTMP_{0}File'.format(tower), 
                          type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftk', treeNames='ftkdata'),
                          help='FTK NTUP file from tower {0}  (for testing only)'.format(tower), group='TrigFTKMerge')
        
    parser.add_argument('--outputNTUP_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='FTK merged tracks file', group='TrigFTKMerge') 
    parser.add_argument('--FTKUnmergedInputPath', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')

    parser.add_argument('--FTKForceAllInput', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')

    parser.add_argument('--FTKDoGrid', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')
    

if __name__ == '__main__':
    main()
