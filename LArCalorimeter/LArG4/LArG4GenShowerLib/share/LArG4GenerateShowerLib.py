#!/usr/bin/env physh
# print command line for logging
from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog", version="%prog $Id: LArG4GenerateShowerLib.py 767177 2016-08-10 08:49:45Z disimone $")

parser.add_option("--inputEvtFileList",    dest="inputevt",    action="append",                       help="select the input file name")
parser.add_option("--inputStructFileList", dest="inputstruct", action="append",                       help="select the output file name")
parser.add_option("--nevents",    dest="nevents",                  type=int,   help="select the number of events to generate (default: all)")
parser.add_option("--skipevents", dest="skipevents",               type=int,   help="select the number of events to skip (default: 0)")
parser.add_option("--geometry",   dest="geometry",                             help="select the geometry tag (default can be used)")
parser.add_option("--condition",  dest="condition",                            help="select the condition tag (REQUIRED)")
parser.add_option("--physlist",   dest="physlist",                             help="select the physics list (default can be used)")
parser.set_defaults(inputevt=[],inputstruct=[],nevents=-1,skipevents=0,geometry="",condition="")
(options, args) = parser.parse_args()

if len(options.inputevt) == 0 :
    print "No input, aborting"
    import sys
    sys.exit(1)

if len(options.condition) == 0 :
    print "No condition tag set, aborting"

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = options.condition

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.CalibrationRun.set_Off()
simFlags.EventFilter.set_off()

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
#simFlags.SimLayout.set_On()
## Set a specific layout tag:
if len(options.geometry) > 0 :
    simFlags.SimLayout = options.geometry
else :
    simFlags.SimLayout.set_On()

if len(options.physlist) > 0 :
    simFlags.PhysicsList = options.physlist

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = options.nevents
athenaCommonFlags.SkipEvents = options.skipevents
athenaCommonFlags.PoolHitsOutput.set_Off()

athenaCommonFlags.PoolEvgenInput=options.inputevt

## Set the LAr parameterization
simFlags.LArParameterization = 0

# get service manager
from AthenaCommon.AppMgr import ServiceMgr

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
from random import randint
simFlags.RandomSeedOffset = randint(1,443921180)

#add G4 function

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::TestActionShowerLibTool',['Event','Run','Step'])

topSeq += PyG4AtlasAlg()

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("LArG4GenShowerLib")
topSeq.LArG4GenShowerLib.LibStructFiles = options.inputstruct
