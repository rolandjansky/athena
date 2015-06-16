#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## AODMergeEI_tf.py - AOD merger with optional TAG, EeventIndex and DPD outputs
# Include modifications to produce EventIndex information
# To be integrated into standard AODMerge_tf.py
# Javier.Sanchez@ific.uv.es

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, hybridPOOLMergeExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

ListOfDefaultPositionalKeys=['--DBRelease', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--fastPoolMerge', '--geometryVersion', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputAODFile', '--maxEvents', '--noimf', '--notcmalloc', '--outputAOD_MRGFile', '--outputTAGFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation']

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s' % sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])

    # EventIndex: update datasetname if outputfile has the form "datasetname#filename"
    # and it has not been specified within the options.
    args = trf.argdict
    if not args.has_key("eidsname"):
        dsname=None
        for filetype in ('AOD_MRG', 'EI'):
            if args.has_key('output'+filetype+'File'):
                outputFile = args['output'+filetype+'File']
                dsname = outputFile.dataset
                break
        if dsname is not None:
            obj=trfArgClasses.argString(dsname)
            args['eidsname'] = obj

    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
    from EventIndexProducer.EITransformUtils import addEI_MRG_Substep,  addEI_MRG_arguments
    executorSet.add(hybridPOOLMergeExecutor(name = 'AODMerge', skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                                   inData = ['AOD'], outData = ['AOD_MRG'], perfMonFile = 'ntuple_POOLMerge.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'AODtoTAG', skeletonFile = 'RecJobTransforms/skeleton.AODtoTAG_tf.py',
                                   inData = ['AOD_MRG'], outData = ['TAG'],))

    addEI_MRG_Substep(executorSet)
    trf = transform(executor = executorSet)
    
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addMyArgs(trf.parser)
    addEI_MRG_arguments(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('AODMerge_tf', 'AOD merge job specific options')
    parser.add_argument('--inputAODFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', type='aod'),
                        help='Input AOD file', group='AODMerge_tf')
    parser.add_argument('--inputAOD_MRGFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', type='aod'),
                        help='Input (premarged) AOD file', group='AODMerge_tf')
    parser.add_argument('--outputAOD_MRGFile', '--outputAODFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', type='aod'),
                        help='Output merged AOD file', group='AODMerge_tf')
    parser.add_argument('--outputTAGFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='output', type='tag'), 
                        help='Output TAG file', group='AODMerge_tf')
    parser.add_argument('--fastPoolMerge', type=trfArgClasses.argFactory(trfArgClasses.argBool),
                        help='Hybrid POOL merging switch (default True)')


if __name__ == '__main__':
    main()
