#!/usr/bin/env python

## FTK Simulation Transform

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

# ___________________________________________________________________________ #
def main():
    
    msg.info('This is %s' % sys.argv[0])
        
    trf = getTransform() 
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)



# ___________________________________________________________________________ #
def getTransform():
# Get the base transform with all arguments added
    trf = transform(executor = athenaExecutor(name = 'FTKPattGenRoot', 
                                              skeletonFile = 'TrigFTKBankGen/skeleton.FTKPattBankGenRoot.py'))
    addAthenaArguments(trf.parser)
    addFTKPattGenArgs(trf.parser)
    return trf



# ___________________________________________________________________________ #
def addFTKPattGenArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('FTKPattGenRoot', 'Options for FTKPattGenRootAlgo for pattern generation')

    # --- 'number of calls of trackInvertion '
    parser.add_argument('--maxEvents', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of events to be processed', 
                        group='FTKPattGenRoot')

    # --- output file (optional)
    parser.add_argument('--OutputFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Name of the output file', 
                        group='FTKPattGenRoot')

    # --- path's to maps
    parser.add_argument('--pmap_path', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of pmap file', 
                        group='FTKPattGenRoot')

    parser.add_argument('--rmap_path', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of rmap file', 
                        group='FTKPattGenRoot')

    parser.add_argument('--ssmap_path', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of ssmap file', 
                        group='FTKPattGenRoot')
    
    # --- path's to constants
    parser.add_argument('--slices', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Slices file', 
                        group='FTKPattGenRoot')
    parser.add_argument('--fitconsts', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Fit-constants path file', 
                        group='FTKPattGenRoot')

    parser.add_argument('--sectors', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='sectors  path file', 
                        group='FTKPattGenRoot')

    # --- patt-gen options
    parser.add_argument('--curreg', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Current region', 
                        group='FTKPattGenRoot')

    parser.add_argument('--seed', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='primary seed of the random generator', 
                        group='FTKPattGenRoot')
    parser.add_argument('--seed2', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='secondary seed of the random generator', 
                        group='FTKPattGenRoot')

    parser.add_argument('--hwmodeid', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Set format of the SS-ID.', 
                        group='FTKPattGenRoot')

    parser.add_argument('--ModuleLUTPath', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='look-up table for HWMODEID=2',
                        group='FTKPattGenRoot')

    parser.add_argument('--IBLMode', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Set IBL Mode. Options are 0, 1 or 2.', 
                        group='FTKPattGenRoot')

    parser.add_argument('--FixEndcapL0', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Endcap layer fix, 0 or 1.', 
                        group='FTKPattGenRoot')

    parser.add_argument('--ITkMode', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Run assuming ITk geometry', 
                        group='FTKPattGenRoot')
    
    parser.add_argument('--pconst_overlap', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Overlap removal: 0=disabled; 1=barrel-only, 2=everywhere', 
                        group='FTKPattGenRoot')

#    parser.add_argument('--smear', 
#                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
#                        help='Boolean flag to set the smearing', 
#                        group='FTKPattGenRoot')

    parser.add_argument('--trials', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of trials of the trackInvertion function', 
                        group='FTKPattGenRoot')

    # --- further options
    parser.add_argument('--phi_min', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter phi min', 
                        group='FTKPattGenRoot')

    parser.add_argument('--phi_max', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter phi max', 
                        group='FTKPattGenRoot')

    parser.add_argument('--c_min', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter c min', 
                        group='FTKPattGenRoot')

    parser.add_argument('--c_max', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter c max', 
                        group='FTKPattGenRoot')

    parser.add_argument('--d0_min', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter d0 min', 
                        group='FTKPattGenRoot')

    parser.add_argument('--d0_max', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter d0 max', 
                        group='FTKPattGenRoot')

    parser.add_argument('--z0_min', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter z0 min', 
                        group='FTKPattGenRoot')

    parser.add_argument('--z0_max', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter z0 max', 
                        group='FTKPattGenRoot')

    parser.add_argument('--eta_min', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter eta min', 
                        group='FTKPattGenRoot')

    parser.add_argument('--eta_max', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='Slice parameter eta max', 
                        group='FTKPattGenRoot')

    parser.add_argument('--d0_alpha', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='exponent for d0 distribution', 
                        group='FTKPattGenRoot')

    parser.add_argument('--keep7of8', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='bit-mask of allowed missing planes for 7/8', 
                        group='FTKPattGenRoot')

    parser.add_argument('--tolerance7of8', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='tolerance for 7/8 option', 
                        group='FTKPattGenRoot')
    parser.add_argument('--beamspotX', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='beamspot (X)', 
                        group='FTKPattGenRoot')
    parser.add_argument('--beamspotY', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='beamspot (Y)', 
                        group='FTKPattGenRoot')

    parser.add_argument('--ModuleGeometryFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='module positions for improving the slices', 
                        group='FTKPattGenRoot')

    parser.add_argument('--sectorSelection', 
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='sector selection algorithm (0:loose .. 3:tight)', 
                        group='FTKPattGenRoot')
    
#    parser.add_argument('--pattgenalgo', 
#                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
#                        help='Algorithm type: 1=random sector selection; 2=weighted random sector selection; 3=all patterns of all sectors; 4=random pattern selection; 5=weighted random pattern selection', 
#                        group='FTKPattGenRoot')
    

if __name__ == '__main__':
    main()
