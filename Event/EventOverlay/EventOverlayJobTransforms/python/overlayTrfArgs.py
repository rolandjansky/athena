# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfArgClasses import argFactory, argFile, argInt, argString, argSubstep, argBool, argBSFile, argRDOFile, argSubstepInt

## Arguments whose default properties have to be overridden for the Overlay Chain
def addOverlayChainOverrideArgs(parser):
    parser.defineArgGroup('Overlay Chain', 'Overlay Chain transform arguments')
    parser.add_argument('--skipEvents', group='Overlay Chain', type=argFactory(argSubstepInt, defaultSubstep='EVNTtoHITS'),
                        help='Number of events to skip over in the EVNTtoHITS processing step')

## Add Overlay transform arguments to an argparse ArgumentParser
def addOverlayBSFilterArgs(parser):
    parser.defineArgGroup('Overlay Filter', 'Overlay filter transform arguments')
    parser.add_argument('--inputBSCONFIGFile',
                        type=argFactory(argFile, io='input', type='TAR', subtype='TAR_CONFIG'),
                        help='Input overlay configuration tarball file', group='Overlay Filter')
    parser.add_argument('--inputZeroBiasBSFile', nargs='+',
                        type=argFactory(argBSFile, io='input', type='BS', subtype='BS_ZeroBias'),
                        help='Input overlay BS file(s)', group='Overlay Filter')
    parser.add_argument('--inputFilterFile',
                        type=argFactory(argString),
                        help='Input overlay BS filter file (or trigger file)', group='Overlay Filter')
    parser.add_argument('--lumiBlockMapFile',
                        type=argFactory(argString),
                        help='Lumi block information file', group='Overlay Filter')
    parser.add_argument('--outputBS_SKIMFile', 
                        type=argFactory(argBSFile, io='output', type='BS', subtype='BS_SKIM'),
                        help='Output skimmed BS file', group='Overlay Filter')
    parser.add_argument('--inputBS_SKIMFile', 
                        type=argFactory(argBSFile, io='input', type='BS', subtype='BS_SKIM'),
                        help='Input skimmed BS file', group='Overlay Filter')
    parser.add_argument('--outputBS_TRIGSKIMFile', '--outputBSTRIGFile',
                        type=argFactory(argBSFile, io='output', type='BS', subtype='BS_TRIGSKIM'),
                        help='Output skimmed BS file', group='Overlay Filter')
    for n in range(1,50):
        myn=str(n)
        parser.add_argument('--outputBS_TRIGSKIM'+myn+'File', '--outputBSTRIG'+myn+'File',
                        type=argFactory(argBSFile, io='output', type='BS', subtype='BS_TRIGSKIM'+myn),
                        help='Output skimmed BS file '+myn, group='Overlay Filter')

    parser.add_argument('--noutputs', type=argFactory(argInt), default='1',
                        help='Number of output skim files', group='Overlay Filter')

    parser.add_argument('--HIfiltervtxdirectory',
                        type=argFactory(argString),
                        help='The directory containing the filter/vtx.data.txt files for the HITAR creation during HI MinBias skimming', group='Overlay Filter')
    parser.add_argument('--outputTXT_EVENTIDFile',
                        type=argFactory(argFile, io = 'output', type='TXT', subtype='TXT_EVENTID'), 
                        help='file for EventIdModifierSvc config for the EVNTtoHITS step', group='Overlay Filter')
    parser.add_argument('--outputTAR_CONFIGFile',
                        type=argFactory(argFile, io = 'output', type='TAR', subtype='TAR_CONFIG'), 
                        help='Out TAR file of filter/vtx.data.txt files for the skimmed events', group='Overlay Filter')
    parser.add_argument('--jobNumber',
                        type=argFactory(argInt),
                        help='Job number', group='Overlay Filter')
    parser.add_argument('--triggerBit',
                        type=argFactory(argSubstepInt, defaultSubstep='overlayBSFilt'),
                        help='Trigger bit', group='Overlay Filter')
    parser.add_argument('--maxFilesPerSubjob',
                        type=argFactory(argSubstepInt, defaultSubstep='overlayBSFilt'),
                        help='Number of bytestream input files for each athena subjob', group='Overlay Filter')
    parser.add_argument('--WriteRDOFileMetaData',
                        type=argFactory(argBool), default = 'false',
                        help='Write the /Digitization/Parameters/ metadata into the RDO file', group='Overlay Filter')
   
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
    parser.add_argument('--outputRDO_SGNLFile', nargs='+',
                        type=argFactory(argRDOFile, io='output'),
                        help='The RDO file of the MC signal alone', group='EventOverlay')
    parser.add_argument('--fSampltag',
                        type=argFactory(argSubstep, defaultSubstep='overlayBS'),
                        help='The cool tag for /LAR/ElecCalib/fSampl/Symmetry, see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LArCalibMCPoolCool', group='EventOverlay')
    #parser.add_argument('--muonForceUse',
    #                    type=argFactory(argBool),
    #                    help='A hack for muon 8-fold versus 12-fold problems?', group='EventOverlay')
    #parser.add_argument('--cscCondOverride',
    #                    type=argFactory(argBool),
    #                    help='A hack to override CSC conditions folder', group='EventOverlay')

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


#Add input txt file to EVNTtoHITS step
def addOverlayInputSimArgs(parser):
    parser.defineArgGroup('EventOverlayInputSim', 'EventOverlayInputSim')
    parser.add_argument('--inputTXT_EVENTIDFile', nargs='+',
                    type=argFactory(argFile, io='input', type='TXT', subtype='TXT_EVENTID'),
                    help='The name of the file to read to configure the EventIdModifierSvc', group='EventOverlayInputSim')
    parser.add_argument('--hitarFile',
                    type=argFactory(argFile, io='input', type='TAR', subtype='TAR_CONFIG'),
                    help='Input Heavy Ion vertex and filter tarball file', group='EventOverlayInputSim')
    parser.add_argument('--inputVertexPosFile',
                    type=argFactory(argString),
                    help='Input overlay file with vertex positions', group='EventOverlayInputSim')

