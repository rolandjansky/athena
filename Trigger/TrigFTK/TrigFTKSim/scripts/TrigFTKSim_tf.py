#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Transform
# @version $Id: TrigFTKSim_tf.py 788284 2016-12-06 19:04:09Z sschmitt $

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
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
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


## Get the base transform with all arguments added
def getTransform():
    trf = transform(executor = athenaExecutor(name = 'FTKFullSimulation',disableMP=True,
                                              skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneSim.py'))
    addAthenaArguments(trf.parser)
    addFTKSimulationArgs(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation options')

    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of pattern banks', group='TrigFTKSim')
    parser.add_argument('--NSubRegions', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of sub-regions', group='TrigFTKSim')

    parser.add_argument('--pmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of pmap file', group='TrigFTKSim')
    parser.add_argument('--pmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of pmapunused file', group='TrigFTKSim')
    parser.add_argument('--pmapcomplete_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of 11L pmap file', group='TrigFTKSim')
    parser.add_argument('--rmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of rmap file', group='TrigFTKSim')
    parser.add_argument('--loadHWConf_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of HW configuration file', group='TrigFTKSim')
    parser.add_argument('--HWNDiff', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
        help="Hit Warrior threshold", group='TrigFTKSim')
    parser.add_argument('--HitWarrior', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
        help="Hit Warrior type: 0 no HW, 1 in sector, 2 global (def)", group='TrigFTKSim')

    parser.add_argument('--ssmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of ssmap file', group='TrigFTKSim')
    parser.add_argument('--ssmaptsp_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of ssmaptsp file', group='TrigFTKSim')
    parser.add_argument('--ssmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of ssmapunused file', group='TrigFTKSim')
    parser.add_argument('--IBLMode',type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help='Enalbe the IBL geometry',group='TrigFTKSim')
    parser.add_argument('--badmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of badmap file', group='TrigFTKSim')
    parser.add_argument('--badmap_path_for_hit', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of badmap file for hits', group='TrigFTKSim')
    parser.add_argument('--UseTSPBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='TSP bank utilisation', group='TrigFTKSim')
    parser.add_argument('--UseCompressedBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='use compressed bank algorithm', group='TrigFTKSim')
    parser.add_argument('--DBBankLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='DBBankLevel', group='TrigFTKSim')
    parser.add_argument('--TSPSimulationLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='TSPSimulationLevel', group='TrigFTKSim')
    parser.add_argument('--TSPMinCoverage', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='TSPMinCoverage', group='TrigFTKSim')

    parser.add_argument('--MakeCache', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Enable MakeCache', group='TrigFTKSim')
    parser.add_argument('--CachePath', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='CachePath', group='TrigFTKSim')
    parser.add_argument("--CachedBank", type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Interpret the pattern bank has a cache", group="TrigFTKSim")
    parser.add_argument("--FixEndCapL0", type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Fix problem with pixel layer assignment in endcap", group="TrigFTKSim")
    parser.add_argument("--SectorsAsPatterns", type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="If 1 allows to use a list of sectors as pattern bank, default 0")

    parser.add_argument('--DoRoadFile', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Enable roads file', group='TrigFTKSim')
    parser.add_argument('--RoadFilesDir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='RoadFilesDir', group='TrigFTKSim')
    parser.add_argument('--SaveRoads', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Save roads file', group='TrigFTKSim')
    parser.add_argument('--patternbankpath', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='Pattern bank path file', group='TrigFTKSim', nargs='+')
    parser.add_argument('--fitconstantspath', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='Fit-constants path file', group='TrigFTKSim', nargs='+')
    parser.add_argument('--fit711constantspath', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='Fit-constants second stage path file', group='TrigFTKSim', nargs='+')
    parser.add_argument('--sectorpath', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='sectors  path file', group='TrigFTKSim', nargs='+')
    parser.add_argument('--bankregion', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                        help='Bank region number', group='TrigFTKSim', nargs='+')
    parser.add_argument('--banksubregion', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                        help='Bank sub-region number', group='TrigFTKSim', nargs='+')
    parser.add_argument('--bankpatterns', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                        help='Number of bank patterns', group='TrigFTKSim', nargs='+')
    parser.add_argument('--SetAMSize',type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='This variable decides how to set the limit on the number of patterns: 0 TSP, 1 or 2 AM (as soon as limit reached, before exceeded)',
                        group='TrigFTKSim')
    parser.add_argument('--SecondStageFit', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Enables the second stage fitter", group='TrigFTKSim')
    parser.add_argument('--doAuxFW', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Enables firmware-style constants", group='TrigFTKSim')
    parser.add_argument('--PrintSSBConstants', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Prints hardware-style SSB constants", group='TrigFTKSim')
    parser.add_argument("--PixelClusteringMode",type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help="Set the pixel clustering mode: 0 default, 1 ToT+pixel center",group="TrigFTKSim")
    parser.add_argument("--Ibl3DRealistic",type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=True),
                        help="Do the IBL 3D realistic(HWlike) clustering? Default is false",group="TrigFTKSim")
    parser.add_argument('--SctClustering',type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=False),
                                help="Set the SCT clustering [def: False]", group="TrigFTKSim")

    parser.add_argument('--TRACKFITTER_MODE', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Second-Stage track fitter mode', group='TrigFTKSim')
    parser.add_argument('--SSFMultiConnection', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Flag to enable the multi-connections mode in the SSF', group='TrigFTKSim')
    parser.add_argument('--SSFNConnections', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Maximum number of connections in the SSF', group='TrigFTKSim')
    parser.add_argument('--SSFAllowExtraMiss', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Allowing extra missed layer in the transition region', group='TrigFTKSim')
    parser.add_argument('--SSFTRDefn', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='How the transition region is defined: 0=moduleID of last SCT layer, 1=by eta of 1st stage track', group='TrigFTKSim')
    parser.add_argument('--SSFTRMinEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True),
                        help='when SSFTRDefn=1 (by eta), the min eta', group='TrigFTKSim')
    parser.add_argument('--SSFTRMaxEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True),
                        help='when SSFTRDefn=1 (by eta), the max eta', group='TrigFTKSim')

    parser.add_argument('--inputTXT_FTKIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, runarg=True, io='input', type='txt_ftkip'),
                        help='Wrapper files (in .dat.bz2 format)', group='TrigFTKSim', nargs='+')
    parser.add_argument('--inputNTUP_FTKIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptmp', treeNames='ftkhits'),
                        help='ROOT format wrapper file path', group='TrigFTKSim', nargs='+')
    parser.add_argument('--outputNTUP_FTKTMPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftktmp', treeNames='ftkdata'),
                        help='Output file path', group='TrigFTKSim')

    parser.add_argument('--inputRDOFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='rdo'),
                        help='Input RDO file', group='TrigFTKSim')


    parser.add_argument('--FTKSetupTag', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Setup the FTK architecture tag, if specific values are also used they have priority', group='TrigFTKSim')
    #JDC:
    parser.add_argument('--ConstantsDir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Directory where input files are kept', group='TrigFTKSim')
    parser.add_argument('--FitConstantsVersion', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Version of fit constants', group='TrigFTKSim')
    parser.add_argument('--PatternsVersion', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Version of patterns', group='TrigFTKSim')
    parser.add_argument('--separateSubRegFitConst', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Use separate fit constant for each subregion', group='TrigFTKSim')
    parser.add_argument('--useDBPath', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Query the Data Base to get File Paths', group='TrigFTKSim')
    parser.add_argument('--runNum', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Run Number', group='TrigFTKSim')
    parser.add_argument('--versionTag', default=0, type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='COOL tag for a different version', group='TrigFTKSim')
    parser.add_argument('--HWModeSS', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='SSID encoding scheme', group='TrigFTKSim')
    parser.add_argument('--ModuleLUTPath', default=0, type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='module LUT for HWModeSS=2', group='TrigFTKSim')
    parser.add_argument('--ModuleLUTPath2nd', default=0, type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='module LUT for HWModeSS=2, unused layers', group='TrigFTKSim')
    parser.add_argument('--MaxMissingSCTPairs', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='6/8 option for transition region', group='TrigFTKSim')
    parser.add_argument('--Save1stStageTrks', default=False, type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='keep 1st stage tracks', group='TrigFTKSim')
    parser.add_argument('--dumpModuleGeom', default=False, type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='dump module geometry', group='TrigFTKSim')
    #end JDC


if __name__ == '__main__':
    main()
