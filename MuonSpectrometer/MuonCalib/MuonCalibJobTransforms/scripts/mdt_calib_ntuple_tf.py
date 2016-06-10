#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """mdt_calib_ntuple_tf.py: Produce calibration ntuples for MDT calibrations.
Use calibration stream as input file and produce calibrations ntuples as output files."""

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments,  addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses
from PyJobTransforms.trfArgClasses import argFactory

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
  executorSet.add(athenaExecutor(name='MDTCalibNtuple', skeletonFile='MuonCalibJobTransforms/skeleton.mdt_calib_ntuple_tf.py' )) 
# possibly (?) one should specify input/output formats to the transform can check them, however, adding this
# info causes transform to fail.   Of course transform does not know about calibstream bytestream, though
# in principle it knows about calib ntuples.
#', inData=['BS'], outData=['NTUP_MUONCALIB']    #specify input/output file formats.
  trf = transform(executor = executorSet)
 
# for maxEvents skipEvents - actually skipEvents do not work with calibstream  
# so do not use addAthenaArguments, but make own arguments for these.
#  addAthenaArguments(trf.parser)
# for runNumber geometryVersion DBRelease conditionsTag
  addDetectorArguments(trf.parser)
# for parameters specific to calib ntuple production
  addMyArgs(trf.parser)
  return trf

def addMyArgs(parser):
  # Use arggroup to get these arguments in their own sub-section (of --help)
  parser.defineArgGroup('mdt_calib_ntuple_tf', 'MDT calib ntuple job specific options')
  parser.add_argument('--inputCSFiles', type=argFactory(trfArgClasses.argList), 
                      help='Input calibration stream file(s)', group='mdt_calib_ntuple_tf')
  parser.add_argument('--ntupleFile', type=argFactory(trfArgClasses.argString), 
                      help='Output calib ntuple', group='mdt_calib_ntuple_tf')

# Cannot use normal maxEvents and skipEvents because these seem to be
# reserved for special use by transform framework, hence use different argument names.
  parser.add_argument("--Events", group="mdt_calib_ntuple_tf",
                     default=trfArgClasses.argInt(-1, runarg=True),
                     help="the number of events to process",
                     type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))
  parser.add_argument("--Skip", group="mdt_calib_ntuple_tf",
                     default=trfArgClasses.argInt(0, runarg=True),
                     help="the number of events to skip at beginning of the file",
                     type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))

  parser.add_argument('--dbT0Tag', type=argFactory(trfArgClasses.argString), default='DEFAULT',
                      help='Specify T0 Tag', group='mdt_calib_ntuple_tf')
  parser.add_argument('--dbRTTag', type=argFactory(trfArgClasses.argString), default='DEFAULT', 
                      help='Specify RT Tag', group='mdt_calib_ntuple_tf')
  parser.add_argument('--dbT0Sql', type=argFactory(trfArgClasses.argString), default='DEFAULT', 
                      help='Specify SQLite file for T0s', group='mdt_calib_ntuple_tf')
  parser.add_argument('--dbRTSql', type=argFactory(trfArgClasses.argString), default='DEFAULT', 
                      help='Specify SQLite file for RTs', group='mdt_calib_ntuple_tf')
# Transform boolean variables will not be created in runArgs unless you specify them on command line
# Hence, in the skeleton you need to check if they exist before you use them.
  parser.add_argument('--useAlign', type=argFactory(trfArgClasses.argBool), 
                      help='Use Alignment corrections', group='mdt_calib_ntuple_tf')
#  segOnly False results in calib ntuple not being filled
#  parser.add_argument('--segOnly', type=argFactory(trfArgClasses.argBool), 
#                      help='Segments only', group='mdt_calib_ntuple_tf')
#  Only standalone exists anymore and is enabled by default so there is no reason to set it anymore
#  parser.add_argument('--moore', type=argFactory(trfArgClasses.argBool), 
#                      help='Use Moore segments + tracks', group='mdt_calib_ntuple_tf')
#  parser.add_argument('--mboy', type=argFactory(trfArgClasses.argBool), 
#                      help='Use Muonboy segments + tracks', group='mdt_calib_ntuple_tf')
#  parser.add_argument('--standalone', type=argFactory(trfArgClasses.argBool), 
#                      help='Use Standalone tracks', group='mdt_calib_ntuple_tf')
  parser.add_argument('--doSegmentT0Fit', type=argFactory(trfArgClasses.argBool),
                      help='Do Segment T0 Fits', group='mdt_calib_ntuple_tf')
  parser.add_argument('--doCSC', type=argFactory(trfArgClasses.argBool), 
                      help='Do CSC', group='mdt_calib_ntuple_tf')
  parser.add_argument('--rawTgc', type=argFactory(trfArgClasses.argBool), 
                      help='put rawTgc and rawTgcCoin branches in calib ntuple', group='mdt_calib_ntuple_tf')
#  doSim is not even used in the skeleton, so skip it.
#  parser.add_argument('--doSim', type=argFactory(trfArgClasses.argBool), 
#                      help='Do Simulated data (whatever that means)', group='mdt_calib_ntuple_tf')
#  Not sure what the use case is for these, but leave them be.
  parser.add_argument('--lumiBlockNumberFromCool', type=argFactory(trfArgClasses.argBool), 
                      help='Fetch Lumiblock Number from COOL', group='mdt_calib_ntuple_tf')
  parser.add_argument('--runNumberFromCool', type=argFactory(trfArgClasses.argBool), 
                      help='Fetch Run Number from COOL (?)', group='mdt_calib_ntuple_tf')
#  Old TRF had this but I do not think it is needed anymore
#        self.add( JobConfigArg(package='MuonCalibJobTransforms'), default='NONE' )

if __name__ == '__main__':
    main()
