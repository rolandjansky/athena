#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """JobTransform to run LAr CAF jobs"""


import sys
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
import PyJobTransforms.trfArgClasses as trfArgClasses

if __name__ == '__main__':

    executorSet = set()
    executorSet.add(athenaExecutor(name = 'LArCAFcalib', skeletonFile = None,
                                   skeletonCA='LArCafJobs.LArShapeDumperSkeleton',
                                   substep = 'r2e', inData = ['BS',], outData = ['NTUP_SAMPLESMON','NTUP_HECNOISE']))
    
    trf = transform(executor = executorSet)  
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    trf.parser.add_argument('--inputBSFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                            help='Input bytestream file', group='Reco Files')
    
    trf.parser.add_argument('--outputNTUP_SAMPLESMONFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output', treeNames="events"),
                            help='Output LAr Samples Mon file', group='Ntuple Files')

    trf.parser.add_argument('--outputNTUP_HECNOISEFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output'),
                            help='Output HECNoise file', group='Ntuple Files')

    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    
