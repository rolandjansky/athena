#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Archiving transform which will tar or zip input files to an output file
# @version $Id: Archive_tf.py 531319 2012-12-20 09:49:53Z graemes $ 

import os.path
import sys
import time

import logging

# This is a hack for Prodsys I. To generate this list use the --dumpargs option to the transform, then paste in here
# N.B. This all has to go on one line!
ListOfDefaultPositionalKeys=['--compressionType', '--ignoreErrors', '--inputFile', '--omitFileValidation', '--outputFile', '--uploadtoami', '--validation']

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
    trf = transform(executor = archiveExecutor(name = 'Archiver', exe='tar'))

    addMyArgs(trf.parser)
    return trf


def addMyArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Archive_tf', 'Archive transform specific options')
    parser.add_argument('--inputDataFile', '--inputFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='input', type='misc'),
                        help='Input file(s)', group='Archive_tf')
    parser.add_argument('--outputArchFile', '--outputFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFile, io='output', type='misc'),
                        help='Output archive file', 
                        group='Archive_tf')
    parser.add_argument('--compressionType', group='Archive_tf',
                        help='Underlying compression type', choices=['gzip', 'bzip2', 'none'],
                        default='gzip')

if __name__ == '__main__':
    main()
