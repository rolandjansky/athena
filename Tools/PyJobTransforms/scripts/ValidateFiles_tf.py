#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## A transform just getting some default file arguments (only to test file validation )

import sys

from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import logscanExecutor
import PyJobTransforms.trfArgs as trfArgs
import PyJobTransforms.trfArgClasses as trfArgClasses
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():

    trf=getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()
    sys.exit(trf.exitCode)

def getTransform():
    trf=transform(executor = logscanExecutor())
    
    # Mostly reco types...
    addArgs(trf.parser)
    trfArgs.addParallelJobProcessorArguments(trf.parser)

    # Add all known D3PD types
    trfArgs.addD3PDArguments(trf.parser, transform = trf, multipleOK=True)

    return trf
        
def addArgs(parser):
    # N.B. Although the trf does not generate these files, badge them as 'output' to run full validation
    parser.add_argument('--outputBSFile', type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output', type='bs', multipleOK=True), nargs='+')
    parser.add_argument('--outputESDFile', type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', type='esd', multipleOK=True), nargs='+')
    parser.add_argument('--outputAODFile', type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', type='aod', multipleOK=True), nargs='+')
    parser.add_argument('--outputRDOFile', type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', type='rdo', multipleOK=True), nargs='+')
    parser.add_argument('--outputTAGFile', type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='output', type='tag', multipleOK=True), nargs='+')
    parser.add_argument('--outputEVNTFile', type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='output', type='evnt', multipleOK=True), nargs='+')
    parser.add_argument('--outputHISTFile', type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', type='hist', multipleOK=True), nargs='+')
    parser.add_argument('--outputTXT_FTKIPFile', type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, io='output', multipleOK=True), nargs='+')
    parser.add_argument('--outputNTUP_FTKSIMFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output', type='ntup_ftksim', treeNames = ['ftkdata'], multipleOK=True), nargs='+')
    parser.add_argument('--outputEvtFile', type=trfArgClasses.argFactory(trfArgClasses.argHepEvtAsciiFile, io='output', type='evt', multipleOK=True), nargs='+')
    parser.add_argument('--logfile', type=trfArgClasses.argFactory(trfArgClasses.argString))


    # This is a dummy argument which makes the graph tracer code happy
    parser.add_argument('--inputFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, io='input', type='dummy'), default=trfArgClasses.argFile(type='dummy'))


if __name__ == '__main__':
    main()
