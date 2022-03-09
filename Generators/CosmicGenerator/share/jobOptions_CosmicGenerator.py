###############################################################
#
# Job options file for testing ParticleGenerator.
#
#==============================================================

## configuring the Athena application for a 'generator' job
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

## make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# The following is needed to load the Athena Random
# Number Generation Service.
# Creating and configuring the service in one go
from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc(
    Seeds = ["SINGLE 2040160768 443921183"]
    )

## adding the CosmicGenerator alg. to the list of algorithms to be run
from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
topSequence += CosmicGenerator()

## adding the McEventCollection dumper to the list of algs. to be run
from TruthExamples.TruthExamplesConf import DumpMC
topSequence += DumpMC()

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
cosmicGenerator = topSequence.CosmicGenerator
cosmicGenerator.emin = 300
cosmicGenerator.emax = 310
cosmicGenerator.xvert_low =   -10.
cosmicGenerator.xvert_hig =    10.
cosmicGenerator.zvert_low =     0.
cosmicGenerator.zvert_hig =    50.
cosmicGenerator.yvert_val =  5000.
cosmicGenerator.OptimizeForSR1 = 0
#==============================================================
#
# End of job options file
#
###############################################################
