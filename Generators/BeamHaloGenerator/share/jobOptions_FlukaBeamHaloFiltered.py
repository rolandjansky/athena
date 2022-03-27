import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#theApp.EvtMax = 2835459 # All of the events
theApp.EvtMax = 1000   

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
BeamHaloGenerator.generatorSettings = [
  "shape cylinder", 
  "pzLimits 10000.0",       # above 10 GeV
  "zLimits -22600. 22600.", # the length of the cavern 22.6m.
  "rLimits 10. 12500."]     # 10mm from the center of the detector, out to the outer radius. 

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


