#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """JobTransform to run LAr PulseShape jobs"""


import sys
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
import PyJobTransforms.trfArgClasses as trfArgClasses

if __name__ == '__main__':

    executorSet = set()
    executorSet.add(athenaExecutor(name = 'LArPulsetf', skeletonFile='RecJobTransforms/skeleton.RAWtoESD_tf.py',
                                   substep = 'r2e', inData = ['BS',], outData = ['NTUP_SAMPLESMON']))
    
    trf = transform(executor = executorSet)  
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    trf.parser.add_argument('--inputBSFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                            help='Input bytestream file', group='Reco Files')
    
    trf.parser.add_argument('--outputNTUP_SAMPLESMONFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output', treeNames="myTree"),
                            help='Output LAr Samples file', group='Ntuple Files')

    trf.parser.add_argument('--autoConfiguration', group='Common Reco', type=trfArgClasses.argFactory(trfArgClasses.argList), 
                            help='Autoconfiguration settings (whitespace or comma separated)', nargs='+', metavar='AUTOCONFKEY')
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    
