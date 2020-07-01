#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## Archiving transform which will tar or zip input files to an output file
# @version $Id: Archive_tf.py 659213 2015-04-07 13:20:39Z graemes $ 

import sys
import time

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import archiveExecutor
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s', sys.argv[0])

    trf = getTransform() 
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d", sys.argv[0], time.asctime(), trf.exitCode)
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
    executorSet.add(archiveExecutor(name = 'Archiver', exe = 'zip', inData = ['Data'], outData = ['Arch']))
    executorSet.add(archiveExecutor(name = 'Unarchiver', exe = 'unarchive', inData = ['Arch'], outData = ['outNULL']))

    trf = transform(executor = executorSet)
    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Archiver', 'Options')
    parser.defineArgGroup('Unarchiver', 'Options')
    parser.add_argument('--inputDataFile', '--inputFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='input', type='misc'),
                        help='Input file(s)', group='Archiver')
    parser.add_argument('--inputArchFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='input', type='misc'),
                        help='Input archive file', group='Unarchiver')
    parser.add_argument('--path', group='Unarchiver',
                        help='Specifies a different directory to extract to. The default is the current working directory', default='.')
    parser.add_argument('--outputArchFile', '--outputFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='output', type='misc'),
                        help='Output archive file', group='Archiver')

if __name__ == '__main__':
    main()
