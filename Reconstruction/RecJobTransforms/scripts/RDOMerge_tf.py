#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @version $Id: RDOMerge_tf.py 710146 2015-11-24 10:37:59Z graemes $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs
from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs

import PyJobTransforms.trfArgClasses as trfArgClasses

ListOfDefaultPositionalKeys=['--AFPOn', '--ALFAOn', '--DBRelease', '--FwdRegionOn', '--LucidOn', '--ZDCOn', '--amiConfig', '--amiMetadataTag', '--asetup', '--athena', '--athenaopts', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--env', '--eventAcceptanceEfficiency', '--execOnly', '--geometryVersion', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputRDOFile', '--maxEvents', '--noimf', '--notcmalloc', '--outputRDO_MRGFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation']

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

def getTransform():
    executorSet = set()
    executorSet.add(athenaExecutor(name = 'RDOMerge', skeletonFile = 'RecJobTransforms/skeleton.MergeRDO_tf.py',
                                   inData = ['RDO'], outData = ['RDO_MRG']))

    trf = transform(executor = executorSet)
    
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addForwardDetTrfArgs(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('RDOMerge_tf', 'RDO merge job specific options')
    parser.add_argument('--inputRDOFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='input'),
                        help='Input RDO file', group='RDOMerge_tf')
    parser.add_argument('--outputRDO_MRGFile', '--outputRDOFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='output'),
                        help='Output merged RDO file', group='RDOMerge_tf')
#     parser.add_argument('--fastPoolMerge', type=trfArgClasses.argFactory(trfArgClasses.argBool),
#                         help='Hybrid POOL merging switch (default True)')


if __name__ == '__main__':
    main()
