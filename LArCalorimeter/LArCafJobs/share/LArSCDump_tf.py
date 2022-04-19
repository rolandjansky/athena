#!/usr/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """JobTransform to run LAr SuperCells dumping jobs"""


import sys
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
import PyJobTransforms.trfArgClasses as trfArgClasses

if __name__ == '__main__':

    executorSet = set()
    executorSet.add(athenaExecutor(name = 'LArSCDump', skeletonFile = None,
                                   skeletonCA='LArCafJobs.LArSCDumperSkeleton',
                                   substep = 'r2e', inData = ['BS',], outData = ['NTUP_SCMON']))
    
    trf = transform(executor = executorSet)  
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    trf.parser.add_argument('--inputBSFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                            help='Input bytestream file', group='Reco Files')
    
    trf.parser.add_argument('--outputNTUP_SCMONFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output', treeNames="SCDIGITS"),
                            help='Output LAr SuperCells Mon file', group='Ntuple Files')

    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    
