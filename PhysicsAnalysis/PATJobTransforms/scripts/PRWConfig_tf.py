#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Transform for making PRW Config files
# @version $Id: PRWConfig_tf.py  ... author: will $ 

import argparse
import os.path
import sys
import time
import traceback

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
import PyJobTransforms.trfExceptions as trfExceptions
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

## Get the base transform with all arguments added
def getTransform():
    trf = transform(executor = athenaExecutor(name = 'athena', 
                                              skeletonFile='PATJobTransforms/skeleton.AODtoNTUP_PILEUP.py'))

    trf.parser.defineArgGroup("PRWConfig_tf","PRWConfig_tf options")

    #for input and output file lists to be put correctly into the runArgs and tested in the pre and post corruption
    #tests, the format of option must be inputXXXXFile and outputYYYYFile
    trf.parser.add_argument("--inputAODFile",type=trfArgClasses.argFactory(trfArgClasses.argFile,io='input',type='aod',multipleOK=True),nargs='+',help="The input files",group="PRWConfig_tf")
    trf.parser.add_argument("--outputNTUP_PILEUPFile",type=trfArgClasses.argFactory(trfArgClasses.argFile,io='output',type='hist',multipleOK=False),help="The output filename",group="PRWConfig_tf")

    return trf
    

if __name__ == '__main__':
    main()
