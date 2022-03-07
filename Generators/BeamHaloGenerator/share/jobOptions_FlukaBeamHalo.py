import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#theApp.EvtMax = 2835459 # All of the events
theApp.EvtMax = 3000000   

################################################################

ServiceMgr.MessageSvc.OutputLevel = INFO

from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["BeamHalo 4894721 4639635"]

from BeamHaloGenerator.BeamHaloGeneratorConf import BeamHaloGeneratorAlg
BeamHaloGenerator = BeamHaloGeneratorAlg()
BeamHaloGenerator.inputType="FLUKA-RB"
BeamHaloGenerator.inputFile="/pcdisk/pcgeneve01/wbell/data/beamhalo/beamgas/beam-gas_IR1_1GeV_cutoff_D1_field"
BeamHaloGenerator.interfacePlane = -22600.0
BeamHaloGenerator.flipProbability = 0.0 
BeamHaloGenerator.randomStream = "BeamHalo"
BeamHaloGenerator.doMonitoringPlots = True

# The generator settings determine if the event is accepted.
#   * If the allowedPdgId list is not given all particles are accepted.
#   * Limits are in the form of (lower limit, upper limit)
#   * If a limit is not given it is disabled.
#   * If a limit value is -1 then it is disabled.
#   * All limits are checked against |value|
#   * r = sqrt(x^2 + y^2)
BeamHaloGenerator.generatorSettings = []#
##  "shape cylinder", # require trajectory to pass through shape 
#  "allowedPdgId 2212 -2212 2112 -2112", # Just p and n
#  "pxLimits 0. 0.",       # lower and upper limits.  The upper limit is optional
#  "pyLimits 0. 0.",       # lower and upper limits.  The upper limit is optional
##  "pzLimits 50000.0",      # above 50 GeV
#  "energyLimits 0. 0.",   # lower and upper limits.  The upper limit is optional
#  "xLimits 0. 0.",        # lower and upper limits.  The upper limit is optional
#  "yLimits 0. 0.",        # lower and upper limits.  The upper limit is optional
##  "zLimits -4000. 4000.",  # required for shape=cylinder 
#  "ptLimits 0. 0.",       # lower and upper limits.  The upper limit is optional
#  "phiLimits 0. 0.",      # lower and upper limits.  The upper limit is optional
#  "etaLimits 0. 0.",      # lower and upper limits.  The upper limit is optional
##  "rLimits 650. 1950."]    # required for shape=cylinder
#  "weightLimits 0. 0."]   # lower and upper limits.  The upper limit is optional

topSequence += BeamHaloGenerator
print BeamHaloGenerator

# Required when monitoring plots are enabled.
theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "BeamHalo DATAFILE='BeamHalo.root' OPT='RECREATE'"]

# For debugging
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC()

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "evgen.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]


