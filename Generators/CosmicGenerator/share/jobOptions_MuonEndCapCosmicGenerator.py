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
job.CosmicGenerator.emin = 10000;  # 10 GeV
job.CosmicGenerator.emax = 2000*1000; # 2 TeV
job.CosmicGenerator.xvert_low =   -301700.
job.CosmicGenerator.xvert_hig =   298300.
job.CosmicGenerator.zvert_low =   -300000.
job.CosmicGenerator.zvert_hig =   300000.
job.CosmicGenerator.yvert_val =   57300+41000.
job.CosmicGenerator.ctcut     =   0.0
job.CosmicGenerator.OptimizeForMuonEndCap = True
job.CosmicGenerator.Radius    =   12000.
job.CosmicGenerator.Zposition =   14500.

from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()
#==============================================================
#
# End of job options file
#
###############################################################
