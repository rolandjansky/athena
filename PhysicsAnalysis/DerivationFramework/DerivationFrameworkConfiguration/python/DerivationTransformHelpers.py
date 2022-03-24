"""Main derivation transform configuration helpers

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from PyJobTransforms.trfArgClasses import argFactory, argList, argPOOLFile
from PyJobTransforms.trfExe import reductionFrameworkExecutor

def addDerivationArguments(parser):
    """Add common derivation command-line parser arguments."""
    parser.defineArgGroup('Derivation', 'Derivation Options')
    parser.add_argument('--requiredDerivedFormats', nargs='*',
                        type=argFactory(argList),
                        help='List of required D(2)AOD formats',
                        group='Derivation')
    parser.add_argument('--inputAODFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input AOD for DAOD building',
                        group='Derivation')
    parser.add_argument('--inputDAOD_PHYSFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input DAOD_PHYS for D2AOD building',
                        group='Derivation')
    parser.add_argument('--inputEVNTFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input EVNT for DAOD_TRUTHX building',
                        group='Derivation')
    parser.add_argument('--outputDAODFile', nargs='+',
                        type=argFactory(argPOOLFile, io='output'),
                        help='Output DAOD filename stub, DAOD_X will be prepended to it',
                        group='Derivation')
    parser.add_argument('--outputD2AODFile', nargs='+',
                        type=argFactory(argPOOLFile, io='output'),
                        help='Output D2AOD filename stub, D2AOD_X will be prepended to it',
                        group='Derivation') 

def addDerivationSubstep(executor_set):
    # We use the existing DF executor which inherits from the athenaExecutor.
    # It handles the composite output filenames and inserts them into the transform's dataDictionary.
    # If this isn't done the file validation will run over the wrong file name.
    executor = reductionFrameworkExecutor(name='Derivation',
                                          skeletonCA='DerivationFrameworkConfiguration.DerivationSkeleton',
                                          substep='DerivationFramework',
                                          tryDropAndReload=False,
                                          perfMonFile='ntuple.pmon.gz',
                                          inData=['EVNT', 'AOD', 'DAOD_PHYS'],
                                          outData=['DAOD', 'D2AOD'])
    executor_set.add(executor)

