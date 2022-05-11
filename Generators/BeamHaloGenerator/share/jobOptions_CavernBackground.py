import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

theApp.EvtMax = 500000 # All of the events

################################################################

ServiceMgr.MessageSvc.OutputLevel = INFO

from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["BeamHalo 3524752 2645932"]

from BeamHaloGenerator.BeamHaloGeneratorConf import BeamHaloGeneratorAlg
BeamHaloGenerator = BeamHaloGeneratorAlg()
BeamHaloGenerator.inputType="FLUKA-VT"
BeamHaloGenerator.inputFile="/localdisk/wbell/data/cavern-background/4vector_7TeV_R203_205_Z428_610.dat"
BeamHaloGenerator.interfacePlane = 0.0 
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
BeamHaloGenerator.generatorSettings = [#
#  "allowedPdgId 2212 -2212 2112 -2112", # Just p and n
  "pxLimits -1 -1",
  "pyLimits -1 -1",
  "pzLimits -1 -1", 
  "energyLimits -1 -1",
  "xLimits -1 -1",
  "yLimits -1 -1",
  "ptLimits -1 -1",
  "phiLimits -1 -1",
  "etaLimits -1 -1",
  "rLimits -1 -1",
  "weightLimits -1 -1"]

topSequence += BeamHaloGenerator
print BeamHaloGenerator

# Required when monitoring plots are enabled.
theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "BeamHalo DATAFILE='BeamHalo.root' OPT='RECREATE'"]

# For debugging
from TruthExamples.TruthExamplesConf import DumpMC
topSequence += DumpMC()

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "evgen.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]


