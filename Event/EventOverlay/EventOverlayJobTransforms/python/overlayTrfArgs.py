# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import argparse
import logging
msg = logging.getLogger(__name__)
import unittest
import pickle
import os

from PyJobTransforms.trfArgClasses import argFactory, argFile, argInt, argFloat, argString, argSubstep, trfArgParser, argList, argBool, argBSFile, argPOOLFile, argHITSFile, argRDOFile, argSubstepInt, argSubstepBool

## Add Overlay transform arguments to an argparse ArgumentParser
def addOverlayBSFilterArgs(parser):
    parser.defineArgGroup('Overlay Filter', 'Overlay filter transform arguments')
    parser.add_argument('--overlayConfigFile',
                        type=argFactory(argString),
                        help='Input overlay configuration tarball file', group='Overlay Filter')
    parser.add_argument('--inputBSFile', nargs='+',
                        type=argFactory(argBSFile, io='input'),
                        help='Input overlay BS file(s)', group='Overlay Filter')
    parser.add_argument('--lumiBlockMapFile',
                        type=argFactory(argString),
                        help='Lumi block information file', group='Overlay Filter')
    parser.add_argument('--outputBS_SKIMFile', '--outputBSFile',
                        type=argFactory(argBSFile, io='output'),
                        help='Output skimmed BS file', group='Overlay Filter')
    parser.add_argument('--eventIdFile',
                        type=argFactory(argString), help='The name of the file to write to for EventIdModifierSvc lines', group='Overlay Filter')
    parser.add_argument('--jobNumber',
                        type=argFactory(argInt),
                        help='Job number', group='Overlay Filter')
    parser.add_argument('--triggerBit',
                        type=argFactory(argInt),
                        help='Trigger bit', group='Overlay Filter')
    parser.add_argument('--maxFilesPerSubjob',
                        type=argFactory(argInt),
                        help='Number of bytestream input files for each athena subjob', group='Overlay Filter')

# jobNumber=102
# InputDataTarFile=/afs/cern.ch/work/e/efeld/overlay/prep/mytar.tar.gz
# InputLbnMapFile=lbn_anal_map.txt
# InputFileMapFile=filelist.txt
# TriggerBit=240

def addOverlayTrfArgs(parser):
    parser.defineArgGroup('EventOverlay', 'Event Overlay Options')
    parser.add_argument('--ReadByteStream',
                        type=argFactory(argBool),
                        help='Are the real data input files byte stream or POOL?', group='EventOverlay')
    parser.add_argument('--muonForceUse',
                        type=argFactory(argBool),
                        help='A hack for muon 8-fold versus 12-fold problems?', group='EventOverlay')
    parser.add_argument('--outputRDO_SGNLFile', nargs='+',
                        type=argFactory(argRDOFile, io='output'),
                        help='The RDO file of the MC signal alone', group='EventOverlay')
    parser.add_argument('--fSampltag',
                        type=argFactory(argString),
                        help='The cool tag for /LAR/ElecCalib/fSampl/Symmetry, see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LArCalibMCPoolCool', group='EventOverlay')
    parser.add_argument('--cscCondOverride',
                        type=argFactory(argBool),
                        help='A hack to override CSC conditions folder', group='EventOverlay')
def addOverlayBSTrfArgs(parser):
    parser.defineArgGroup('EventOverlayBS', 'Event Overlay Options')
    parser.add_argument('--inputBS_SKIMFile', nargs='+',
                        type=argFactory(argBSFile, io='input'),
                        help='Input RAW BS for pileup overlay', group='EventOverlayBS')

def addOverlayPoolTrfArgs(parser):
    parser.defineArgGroup('EventOverlayPool', 'Event Overlay Options')
    parser.add_argument('--inputRDO_BKGFile', nargs='+',
                        type=argFactory(argRDOFile, io='input'),
                        help='Input RAW RDO for pileup overlay', group='EventOverlayPool')
