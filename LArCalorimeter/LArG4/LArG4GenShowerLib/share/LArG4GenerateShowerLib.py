#!/usr/bin/env physh
# print command line for logging
from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog", version="%prog $Id: LArG4GenerateShowerLib.py 575758 2013-12-16 17:03:28Z gsedov $")

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
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.CalibrationRun.set_Off()

## Layout tags: see SimFlags.SimLayout for allowed values
## Use the default layout:
#SimFlags.SimLayout.set_On()
## Set a specific layout tag:
if len(options.geometry) > 0 :
    SimFlags.SimLayout = options.geometry
else :
    SimFlags.SimLayout.set_On()

if len(options.physlist) > 0 :
    SimFlags.PhysicsList = options.physlist

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = options.nevents
athenaCommonFlags.SkipEvents = options.skipevents
athenaCommonFlags.PoolHitsOutput.set_Off()

athenaCommonFlags.PoolEvgenInput=options.inputevt

## Set the LAr parameterization
SimFlags.LArParameterization = 0

# get service manager
from AthenaCommon.AppMgr import ServiceMgr

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
from random import randint
SimFlags.RandomSeedOffset = randint(1,443921180)

#add G4 function
def add_my_user_action():
	from G4AtlasApps import PyG4Atlas,AtlasG4Eng
	MyAction = PyG4Atlas.UserAction( 'LArG4GenShowerLib','TestActionShowerLib',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
	AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)

SimFlags.InitFunctions.add_function("postInit", add_my_user_action)

topSeq += PyG4AtlasAlg()

from LArG4GenShowerLib.LArG4GenShowerLibConf import LArG4GenShowerLib
topSeq += LArG4GenShowerLib()

topSeq.LArG4GenShowerLib.LibStructFiles = options.inputstruct
