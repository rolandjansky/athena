#!/usr/bin/env physh
# print command line for logging
from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog", version="%prog $Id: LArG4GenerateFSStartingPoints.py 574715 2013-12-09 23:43:19Z gsedov $")

parser.add_option("--inputFile",  dest="input",       default=None, action="append",      help="select the input genevent file name (preferred over the particle gun)")
parser.add_option("--outputFile", dest="output",      default='StartGenEvents',           help="select the output file name (default: %default)")
parser.add_option("--geometry",   dest="geometry",    default='',                         help="select the geometry tag (default can be used)")
parser.add_option("--condition",  dest="condition",   default='',                         help="select the geometry tag (REQUIRED)")
parser.add_option("--physlist",   dest="physlist",                                        help="select the physics list (default can be used)")
parser.add_option("--etaMin",     dest="etaMin",      default='-3.3',                     help="particle gun: Minimum Eta (default: %default)")
parser.add_option("--etaMax",     dest="etaMax",      default='3.3',                      help="particle gun: Maximum Eta (default: %default)")
parser.add_option("--energyMin",  dest="energyMin",   default='20000',                    help="particle gun: particle Minimum energy (default: %default MeV)")
parser.add_option("--energyMax",  dest="energyMax",   default='50000',                    help="particle gun: Maximum energy (default: %default MeV)")
parser.add_option("--pid",        dest="pid",         default='11',                       help="particle gun: particle type (11=electron, 13=muon, 111=pi0 211=pi+, .., default: %default)")
parser.add_option("--nevents",    dest="nevents",     default=5000,           type=int,   help="number of events to process (default: %default)")
parser.add_option("--spratio",    dest="spratio",     default=0.02,           type=float, help="the ratio of starting points to be saved (default: %default)")
(options, args) = parser.parse_args()

if len(options.condition) == 0 :
    print "No condition tag set, aborting"

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Lucid_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = options.condition

## Simulation flags
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.CalibrationRun.set_Off()

if len(options.geometry) > 0 :
    SimFlags.SimLayout = options.geometry
else :
    SimFlags.SimLayout.set_On()

if len(options.physlist) > 0 :
    SimFlags.PhysicsList = options.physlist

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = options.nevents
athenaCommonFlags.PoolHitsOutput.set_Off()

## Set the LAr parameterization
SimFlags.LArParameterization = 1

from random import randint
SimFlags.RandomSeedOffset = randint(1,443921180)

if options.input is not None:
    athenaCommonFlags.PoolEvgenInput=options.input
else:
    ## Use single particle generator
    import AthenaCommon.AtlasUnixGeneratorJob
    spgorders = ['pdgcode: constant '+ options.pid,
                 'vertX: constant 0.0',
                 'vertY: constant 0.0',
                 'vertZ: constant 0.0',
                 't: constant 0.0',
                 'eta: flat ' + options.etaMin + ' ' + options.etaMax,
                 'phi: flat 0 6.28318',
                 'e: flat ' + options.energyMin + ' ' + options.energyMax]

    athenaCommonFlags.PoolEvgenInput.set_Off()
    from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
    topSequence += ParticleGenerator()
    topSequence.ParticleGenerator.orders = sorted(spgorders)
"""
    from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
    if not hasattr(ServiceMgr, 'AtRanluxGenSvc' ) :
        ServiceMgr += AtRanluxGenSvc()
    seed = "SINGLE "+str(randint(1,294967296))+" "+str(randint(1,294967296))
    ServiceMgr.AtRanluxGenSvc.Seeds += [seed]
"""

# get service manager
from AthenaCommon.AppMgr import ServiceMgr

from LArG4FastSimSvc.LArG4FastSimSvcInit import LArG4FastSimSvcInit

LArG4FastSimSvcInit()

ServiceMgr.LArG4EMBFastSimSvc.GeneratedStartingPointsFile="EMB."+options.output
ServiceMgr.LArG4EMBFastSimSvc.GeneratedStartingPointsRatio=options.spratio
ServiceMgr.LArG4EMECFastSimSvc.GeneratedStartingPointsFile="EMEC."+options.output
ServiceMgr.LArG4EMECFastSimSvc.GeneratedStartingPointsRatio=options.spratio
ServiceMgr.LArG4FCALFastSimSvc.GeneratedStartingPointsFile="FCAL1."+options.output
ServiceMgr.LArG4FCALFastSimSvc.GeneratedStartingPointsRatio=options.spratio
ServiceMgr.LArG4FCAL2FastSimSvc.GeneratedStartingPointsFile="FCAL2."+options.output
ServiceMgr.LArG4FCAL2FastSimSvc.GeneratedStartingPointsRatio=options.spratio

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSequence += PyG4AtlasAlg()
