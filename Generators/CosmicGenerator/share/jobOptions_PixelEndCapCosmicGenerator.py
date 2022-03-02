###############################################################
#
# Job options file for testing ParticleGenerator.
#
#==============================================================
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# The following is needed to load the Athena Random
# Number Generation Service.
from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["SINGLE 2040160768 443921183"]
# ServiceMgr.AtRndmGenSvc.ReadFromFile = true;

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = VERBOSE
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 20
ServiceMgr.StoreGateSvc.Dump = FALSE

#--------------------------------------------------------------
# CosmicGenerator parameters
#--------------------------------------------------------------
#
# Note that in this coordinate frame the y-axis points upward
# such that the cosmics arrive from upward to downward in y.
# The production vertex of cosmics is randomly distributed (flat)
# in the x-z plane with boundaries given below.
# The energy range is given as well.
#
from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
job += CosmicGenerator()
job.CosmicGenerator = Algorithm( "CosmicGenerator" )
job.CosmicGenerator.emin = 500
job.CosmicGenerator.emax = 200000
job.CosmicGenerator.xvert_low = -229.
job.CosmicGenerator.xvert_hig = 229.
job.CosmicGenerator.zvert_low = -356.
job.CosmicGenerator.zvert_hig = 356.
# add a centimeter above the scintillator...
# so that muon has full path through the scintillator volume
job.CosmicGenerator.yvert_val = 200.
# remember that theta is measured from the surface of the scintillator
job.CosmicGenerator.ThetaMin  = 1.
job.CosmicGenerator.ThetaMax  = 3.1415927
job.CosmicGenerator.PhiMin    = -3.1415927
job.CosmicGenerator.PhiMax    = 3.1415927
job.CosmicGenerator.OptimizeForSR1PixelEndCap = True
job.CosmicGenerator.SwapYZAxis = True

from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()

#==============================================================
#
# End of job options file
#
###############################################################
