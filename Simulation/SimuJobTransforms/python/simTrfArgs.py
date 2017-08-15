# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import argparse
import logging
msg = logging.getLogger(__name__)
import unittest
import pickle
import os

from PyJobTransforms.trfArgClasses import argFactory, argFile, argInt, argFloat, argString, argSubstep, trfArgParser, argList, argBool, argPOOLFile, argHITSFile, argRDOFile, argSubstepInt, argSubstepBool
#from PyJobTransforms.trfLogger import stdLogLevels
#from PyJobTransforms.trfDecorators import silent
#from PyJobTransforms.trfExitCodes import trfExit
#import PyJobTransforms.trfArgClasses as trfArgClasses

## Add Forward Detector transform arguments to an argparse ArgumentParser
def addForwardDetTrfArgs(parser):
    parser.defineArgGroup('ForwardDetector', 'Forward Detector Options')
    parser.add_argument('--LucidOn',
                        type=argFactory(argBool),
                        help='Switch on lucid simulation/digitization.', group='ForwardDetector')
    parser.add_argument('--AFPOn',
                        type=argFactory(argBool),
                        help='Switch on AFP simulation/digitization.', group='ForwardDetector')
    parser.add_argument('--ALFAOn',
                        type=argFactory(argBool),
                        help='Switch on ALFA simulation/digitization.', group='ForwardDetector')
    parser.add_argument('--ZDCOn',
                        type=argFactory(argBool),
                        help='Switch on ZDC simulation/digitization.', group='ForwardDetector')
    parser.add_argument('--FwdRegionOn',
                        type=argFactory(argBool),
                        help='Switch on FwdRegion simulation/digitization.', group='ForwardDetector')
    parser.add_argument('--HGTDOn',
                        type=argFactory(argBool),
                        help='Switch on HGTD simulation/digitization.', group='ForwardDetector')

## Add Basic digitization arguments to an argparse ArgumentParser
def addBasicDigiArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Digi', 'Digitization specific options')
    parser.add_argument('--inputHITSFile', '--inputHitsFile', nargs='+',
                        type=argFactory(argHITSFile, io='input'),
                        help='Input HITS file', group='Digi')
    parser.add_argument('--outputRDOFile', nargs='+',
                        type=argFactory(argRDOFile, io='output'),
                        help='Output RDO file', group='Digi')
    parser.add_argument('--outputRDO_FILTFile', nargs='+',
                        type=argFactory(argRDOFile, io='output'),
                        help='Output Filtered RDO file', group='Digi')
    parser.add_argument('--digiSeedOffset1',
                        type=argFactory(argInt),
                        help='Offset for first random seed', group='Digi')
    parser.add_argument('--digiSeedOffset2',
                        type=argFactory(argInt),
                        help='Offset for second random seed', group='Digi')
    parser.add_argument('--digiRndmSvc',
                        type=argFactory(argString), ##STRING CHOICES ARG,
                        help='Random Number Service to use.', group='Digi' )
    parser.add_argument('--digiSteeringConf',
                        type=argFactory(argString),
                        help='Required configuration fod the digitization job', group='Digi' )
    parser.add_argument('--samplingFractionDbTag',
                        type=argFactory(argString),
                        help='This argument can be used to override the PhysicsList retrieved from the Simulation metadata. This information is used in the LAr sampling fraction data base tag used by LArfSamplG4Phys.', group='Digi')
    parser.add_argument('--doAllNoise',
                        type=argFactory(argBool),
                        help='Overall control of noise simulation during digitization - useful for overlay jobs, will set doCaloNoise, doMuonNoise, doInDetNoise digitizationFlags.', group='Digi')
    parser.add_argument('--AddCaloDigi',
                        type=argFactory(argBool),
                        help='Save Calo Digits too, not just RawChannels.', group='Digi')
    parser.add_argument('--PileUpPremixing',
                        type=argFactory(argBool),
                        help='Run digitization with pile-up premixing configuration.', group='Digi')

## Add Pile-up related transform arguments to an argparse ArgumentParser
def addPileUpTrfArgs(parser):
    parser.defineArgGroup('PileUp', 'Pile-Up Options')
    parser.add_argument('--testPileUpConfig',
                        type=argFactory(argBool),
                        help='Calculates the number of background events that will be require for a given pile-up configuration.', group='PileUp')
    parser.add_argument('--inputLowPtMinbiasHitsFile','--LowPtMinbiasHitsFile', nargs='+',
                        type=argFactory(argHITSFile, io='input', executor=['HITtoRDO'], auxiliaryFile=True),
                        help='Input HITS file for low pT minimum bias pile-up sub-events', group='PileUp')
    parser.add_argument('--inputHighPtMinbiasHitsFile','--HighPtMinbiasHitsFile', nargs='+',
                        type=argFactory(argHITSFile, io='input', executor=['HITtoRDO'], auxiliaryFile=True),
                        help='Input HITS file for high pT minimum bias pile-up sub-events', group='PileUp')
    parser.add_argument('--inputCavernHitsFile', '--cavernHitsFile', nargs='+',
                        type=argFactory(argHITSFile, io='input', executor=['HITtoRDO'], auxiliaryFile=True),
                        help='Input HITS file for cavern background sub-events', group='PileUp')
    parser.add_argument('--inputBeamHaloHitsFile', '--beamHaloHitsFile', nargs='+',
                        type=argFactory(argHITSFile, io='input', executor=['HITtoRDO'], auxiliaryFile=True),
                        help='Input HITS file for beam halo sub-events', group='PileUp'),
    parser.add_argument('--inputBeamGasHitsFile', '--beamGasHitsFile', nargs='+',
                        type=argFactory(argHITSFile, io='input', executor=['HITtoRDO'], auxiliaryFile=True),
                        help='Input HITS file for beam gas sub-events', group='PileUp')
    parser.add_argument('--numberOfLowPtMinBias',
                        type=argFactory(argFloat),
                        help='Sets the intial number low pt minbias events to add on average per bunch crossing. Overrides any values set in joboproperties by preIncludes or preExecs. May later be overridden during the job by run-dependent MC code.', group='PileUp')
    parser.add_argument('--numberOfHighPtMinBias',
                        type=argFactory(argFloat),
                        help='Sets the intial number high pt minbias events to add on average per bunch crossing. Overrides any values set in joboproperties by preIncludes or preExecs. May later be overridden during the job by run-dependent MC code.', group='PileUp')
    parser.add_argument('--numberOfBeamHalo',
                        type=argFactory(argFloat),
                        help='Sets the intial number beam halo events to add on average per bunch crossing. Overrides any values set in joboproperties by preIncludes or preExecs. May later be overridden during the job by run-dependent MC code.', group='PileUp')
    parser.add_argument('--numberOfBeamGas',
                        type=argFactory(argFloat),
                        help='Sets the intial number beam gas events to add on average per bunch crossing. Overrides any values set in joboproperties by preIncludes or preExecs. May later be overridden during the job by run-dependent MC code.', group='PileUp')
    parser.add_argument('--numberOfCavernBkg',
                        type=argFactory(argInt),
                        help='Sets the intial number cavern background events to add on average per bunch crossing. Overrides any values set in joboproperties by preIncludes or preExecs. May later be overridden during the job by run-dependent MC code. NB this value is an integer!', group='PileUp')
    parser.add_argument('--bunchSpacing',
                        type=argFactory(argFloat), #FIXME This runArg should probably inherit from argInt rather than argFloat
                        help='Overrides any values of digitizationFlags.bunchSpacing by preIncludes or preExecs. When using bunch structure is being usedthis sets the time between successive elements in the bunch structure pattern. If not using bunch-structure, this can be used to run pile-up with a fixed X ns spacing between filled bunch-crossings.', group='PileUp')
    parser.add_argument('--pileupInitialBunch',
                        type=argFactory(argInt),
                        help='Initial (relative) bunch crossing to use pileup - default -32', group='PileUp')
    parser.add_argument('--pileupFinalBunch',
                        type=argFactory(argInt),
                        help='Initial (relative) bunch crossing to use pileup - default 32', group='PileUp')

## Add Cosmics related transform arguments to an argparse ArgumentParser
def addCosmicsTrfArgs(parser):
    parser.defineArgGroup('Cosmics', 'Cosmic Ray Simulation Options')
    parser.add_argument('--CosmicFilterVolume',
                        type=argFactory(argString),
                        help='Main Cosmic Filter Volume - tracks have to pass through this volume for the event to be written out.', group='Cosmics')
    parser.add_argument('--CosmicFilterVolume2',
                        type=argFactory(argString),
                        help='Secondary Cosmic Filter Volume (Optional) - if specified tracks also have to pass through this volume for the event to be written out.', group='Cosmics')
    parser.add_argument('--CosmicPtSlice',
                        type=argFactory(argString),
                        help='Cosmic Pt Slice', group='Cosmics')

## Add arguments used by simulation jobs which may write out TrackRecord files
def addTrackRecordArgs(parser):
    parser.defineArgGroup('TrackRecords', 'TrackRecord related options')
    parser.add_argument('--inputEVNT_COSMICSFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input Track Record file - sometimes used in Cosmic ray simulation jobs.', group='TrackRecords')
    parser.add_argument('--outputEVNT_COSMICSTRFile', nargs='+',
                        type=argFactory(argPOOLFile, io='output', type='evnt'),
                        help='Output Track Record file - sometimes used in Cosmic ray simulation jobs.', group='TrackRecords')
    parser.add_argument('--inputEVNT_CAVERNFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input Track Record file - sometimes used in Cavern Background simulation jobs.', group='TrackRecords')
    parser.add_argument('--outputEVNT_CAVERNTRFile', nargs='+',
                        type=argFactory(argPOOLFile, io='output', type='evnt'),
                        help='Output Track Record file - sometimes used in Cavern Background simulation jobs.', group='TrackRecords')

## Add arguments used only by ISF-based simulation jobs
def addSim_tfArgs(parser):
    parser.defineArgGroup('Sim_tf', 'Sim_tf specific options')
    parser.add_argument('--simulator',
                        type=argFactory(argString), metavar='CONFIGNAME',
                        help='Specify a named configuration. E.g. MC12G4, ATLFASTII ATLFASTIIF', group='Sim_tf')

## Add arguments used only by TestBeam simulation jobs
def addTestBeamArgs(parser):
    parser.defineArgGroup('TestBeam', 'TestBeam_tf specific options')
    parser.add_argument('--testBeamConfig',
                        type=argFactory(argString), metavar='CONFIGNAME',
                        help='Specify a named test beam configuration. E.g. ctb, tbtile', group='TestBeam')
    parser.add_argument('--Eta',
                        type=argFactory(argFloat),
                        help='Only set Eta if you want to simulate an eta-projective scan', group='TestBeam')
    parser.add_argument('--Theta',
                        type=argFactory(argFloat),
                        help='For 90 degrees scans put Theta=+/-90. Positive theta means the beam enters from positive eta side (as defined in CTB setup!)', group='TestBeam')
    parser.add_argument('--Z',
                        type=argFactory(argFloat),
                        help='Z coordinate is the distance from ATLAS center to the desired impact point. Sensitive part starts at Z=2300, ends at Z=2300+3*100+3*130+3*150+2*190=3820', group='TestBeam')

## Add common Simulation transform arguments to an argparse ArgumentParser
def addCommonSimTrfArgs(parser):
    parser.defineArgGroup('CommonSim', 'Common Simulation Options')
    parser.add_argument('--inputEVNTFile', '--inputEvgenFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input evgen file', group='CommonSim')
    parser.add_argument('--outputHITSFile', '--outputHitsFile', nargs='+',
                        type=argFactory(argHITSFile, io='output'),
                        help='Output HITS file', group='CommonSim')
    parser.add_argument('--firstEvent', metavar='FIRSTEVENT',
                        type=argFactory(argInt), help='The event number to use for the first Event', group='CommonSim')
    parser.add_argument('--physicsList', metavar='PHYSICSLIST',
                        type=argFactory(argString), help='Physics List to be used within Geant4', group='CommonSim')
    parser.add_argument('--useISF',
                        type=argFactory(argBool), help='Use ISF (only will only work from 17.6.0 onwards)', group='CommonSim')
    parser.add_argument('--randomSeed', nargs='+', metavar='substep:seed',
                        type=argFactory(argSubstepInt), help='Random seed offset', group='CommonSim')
    parser.add_argument('--enableLooperKiller', nargs='+', metavar='substep:LooperKiller',
                        type=argFactory(argSubstepBool), help='Should the LooperKiller be used', group='CommonSim')
    parser.add_argument('--truthStrategy',
                        type=argFactory(argString), metavar='CONFIGNAME',
                        help='Specify the named group of Truth strategies that the simulation should use.  E.g. MC12, MC15aPlus, MC16', group='CommonSim')

## Add common Simulation/Digitization transform arguments to an argparse ArgumentParser
def addCommonSimDigTrfArgs(parser):
    parser.defineArgGroup('SimDigi', 'Common Simulation Options')
    parser.add_argument('--DataRunNumber',
                        type=argFactory(argInt),
                        help='Override existing run number with this value - deprecated?', group='SimDigi')
    parser.add_argument('--jobNumber',
                        type=argFactory(argInt),
                        help='The number of this job in the current RunDependentSimulation task.', group='SimDigi')
    parser.add_argument('--eventService',
                        type=argFactory(argBool),
                        help='Switch AthenaMP to the Event Service configuration', group='SimDigi')

def addHITSMergeArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('HITSMerge_tf', 'HITSMerge_tf specific options')
    parser.add_argument('--inputHITSFile', '--inputHitsFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input', runarg=True, type='hits'),
                        help='Input HITS files', group='HITSMerge_tf')
    parser.add_argument('--outputHITS_MRGFile', '--outputHits_MRGFile', '--outputHITSFile', '--outputHitsFile', nargs='+',
                        type=argFactory(argPOOLFile, io='output', runarg=True, type='hits'),
                        help='Output HITS file', group='HITSMerge_tf')
    parser.add_argument('--inputLogsFile', nargs='+',
                        type=argFactory(argFile, io='input', runarg=True, type='log'),
                        help='Input Log files', group='HITSMerge_tf') ## FIXME need to add code to do the log file merging.

## Add HITS validation transform arguments
def addHITSValidArgs(parser):
    parser.defineArgGroup('SimValid_tf', 'SimValid_tf specific options')
    parser.add_argument('--inputHITSFile', nargs = '+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input HITS files', group='SimValid_tf')
    parser.add_argument('--outputHIST_SIMFile', nargs = '+',
                        type=argFactory(argFile, io='output'),
                        help=' Output HIST_SIM files', group='SimValid_tf')

## Add RDO validation transform arguments
def addRDOValidArgs(parser):
    parser.defineArgGroup('DigiValid_tf', 'DigiValid_tf specific options')
    parser.add_argument('--inputRDOFile', nargs = '+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input RDO files', group='DigiValid_tf')
    parser.add_argument('--outputHIST_DIGIFile', nargs = '+',
                        type=argFactory(argFile, io='output'),
                        help=' Output HIST_DIGI files', group='DigiValid_tf')
