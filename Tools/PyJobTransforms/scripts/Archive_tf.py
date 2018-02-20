#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Archiving transform which will tar or zip input files to an output file
# @version $Id: Archive_tf.py 659213 2015-04-07 13:20:39Z graemes $ 

import os.path
import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import archiveExecutor
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

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

def getTransform():
    trf = transform(executor = archiveExecutor(name = 'Archiver', exe = 'zip'))

    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Archive_tf', 'Archive transform options')
    parser.defineArgGroup('Tar archiver', 'Options')
    parser.defineArgGroup('Zip archiver', 'Options')
    parser.add_argument('--exe', group='Archive_tf',
                        help='Archiving command. Default is zip', choices=['zip', 'tar'],
                        default='zip')
    parser.add_argument('--inputDataFile', '--inputFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='input', type='misc'),
                        help='Input file(s)', group='Archive_tf')
    parser.add_argument('--outputArchFile', '--outputFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='output', type='misc'),
                        help='Output archive file', group='Archive_tf')
    parser.add_argument('--compressionType', group='Tar archiver',
                        help='Underlying compression type of tar. Default is none', choices=['gzip', 'bzip2', 'none'],
                        default='none')
    parser.add_argument('--compressionLevel', group='Zip archiver',
                        help='Compression level of zip. Default is -0', choices=['-0', '-1', '-2', '-3',  '-4',  '-5', '-6', '-7', '-8', '-9'],
                        default='-0')

if __name__ == '__main__':
    main()
