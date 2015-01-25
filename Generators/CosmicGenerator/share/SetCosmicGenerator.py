"""
Set up cosmic generator for simulation + trigger.
"""

## Configuring the Athena application for a 'generator' job
from G4AtlasApps.SimFlags import simFlags
simFlags.load_cosmics_flags()
assert hasattr(simFlags, "ReadTR")
if simFlags.ReadTR.statusOn:
    import G4AtlasApps.AtlasCosmicTrackRecordJob
else:
    import AthenaCommon.AtlasUnixGeneratorJob
    
## Set up standard algorithms and random seeds
from AthenaCommon.AppMgr import ServiceMgr
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if hasattr(simFlags, 'RandomSeedList'):
    ##Only for MC12 onwards
    if not simFlags.RandomSeedList.checkForExistingSeed( "COSMICS"):
        simFlags.RandomSeedList.addSeed( "COSMICS", 2040160768, 443921183 )

## Adding the CosmicGenerator to the list of algs
from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
topSequence += CosmicGenerator()

## Adding the McEventCollection dumper to the list of algs
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC()

##--------------------------------------------------------------
## CosmicGenerator parameters
##--------------------------------------------------------------
##
## Note that in this coordinate frame the y-axis points upward
## such that the cosmics arrive from upward to downward in y.
## The production vertex of cosmics is randomly distributed (flat)
## in the x-z plane with boundaries given below.
## The energy range is given as well.
##
## The following settings are tuned to scintillators of dimensions
## 140 x 0.5 x 100 cm^3 placed at +-115.0 cm
##
## NOTE: From G4AtlasApps-04-00-00 onwards, IDET-* cosmics
## commissioning layouts are not supported!

from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType() == "cosmics":
    if hasattr(simFlags, 'RandomSvc'):
        ##Only for MC12 onwards
        topSequence.CosmicGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()
    topSequence.CosmicGenerator.emin = 10000;  # 10 GeV
    topSequence.CosmicGenerator.emax = 2000*1000; # 2 TeV
    topSequence.CosmicGenerator.xvert_low = -301700.
    topSequence.CosmicGenerator.xvert_hig =  298300.
    topSequence.CosmicGenerator.zvert_low = -300000.
    topSequence.CosmicGenerator.zvert_hig =  300000.
    topSequence.CosmicGenerator.yvert_val =  57300 + 41000.
    topSequence.CosmicGenerator.ctcut     =  0.0
    topSequence.CosmicGenerator.OptimizeForCavern= True
    topSequence.CosmicGenerator.IPx = 0.0
    topSequence.CosmicGenerator.IPy = 0.0
    topSequence.CosmicGenerator.IPz = 0.0
    topSequence.CosmicGenerator.Radius = 2000.0
    topSequence.CosmicGenerator.ReadTR = simFlags.ReadTR.statusOn

if simFlags.ReadTR.statusOn:
    topSequence.CosmicGenerator.TRSmearing = -1 #in millimeters, e.g. 10
    topSequence.CosmicGenerator.TRPSmearing = -1 #in radians, e.g. 0.01
    #topSequence.CosmicGenerator.OutputLevel = DEBUG # for turning up output during testing

if simFlags.CosmicPtSlice.statusOn:
    print "Configuring cosmic pT slice: %s" % simFlags.CosmicPtSlice
    include("CosmicGenerator/CosmicSliceConfig.py")

print topSequence.CosmicGenerator
