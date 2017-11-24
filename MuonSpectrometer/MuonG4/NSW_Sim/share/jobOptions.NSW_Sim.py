## Job options file for Geant4 ATLAS detector simulations

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Lucid_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-MC12b-SIM-00"

from AGDD2Geo.AGDD2GeoConf import AGDD2GeoSvc
Agdd2GeoSvc = AGDD2GeoSvc()
Agdd2GeoSvc.PrintSections = True
Agdd2GeoSvc.XMLFiles += ["stations.v1.68.xml"]
Agdd2GeoSvc.Sections += ["NewSmallWheel"]
Agdd2GeoSvc.DisableSections = False
Agdd2GeoSvc.PrintLevel = 1

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E200_eta0-60-10000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = "atlasG4.hits.pool.root"
athenaCommonFlags.EvtMax = 10

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
simFlags.SimLayout.set_On()
## Set a specific layout tag:
#simFlags.SimLayout='ATLAS-GEO-18-01-00'
## Set a specific non-officially-supported layout tag using the _VALIDATION suffix:
simFlags.SimLayout = 'ATLAS-GEO-21-00-01_VALIDATION'

## Alternative to use a common GEO tag but the NSW MS layout
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.06.03-NSW.01.64"

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_On()

## Set the LAr parameterization
#simFlags.LArParameterization = 2

## No magnetic field
#simFlags.MagneticField.set_Off()

simFlags.SimulateNewSmallWheel=True

## Register callback functions at various init stages
#def test_preInit():
#    print "CALLBACK AT PREINIT"
#def test_postInit():
#    print "CALLBACK AT POSTINIT"
#simFlags.InitFunctions.add_function("preInit", test_preInit)
#simFlags.InitFunctions.add_function("postInit", test_postInit)
#def nsw_SetSensitiveDetectors():
#    from G4AtlasApps.atlas_muon import Muon
#    muon=Muon()
#    muon.atlas_muon.add_SenDetector('MuonG4SD', 'sTGCSensitiveDetector' , 'NEW', 'Muon::sTGC_Sensitive')
#    muon.atlas_muon.add_SenDetector('MuonG4SD', 'MicromegasSensitiveDetector' , 'NEU', 'Muon::MM_Sensitive')
#simFlags.InitFunctions.add_function("preInitSenDetector", nsw_SetSensitiveDetectors)

## Change the field stepper or use verbose G4 tracking
#from G4AtlasApps import callbacks
#simFlags.InitFunctions.add_function("postInit", callbacks.use_simplerunge_stepper)
#simFlags.InitFunctions.add_function("postInit", callbacks.use_verbose_tracking)

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 13',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat  1.5 2.0',
             'phi: flat  0 6.28318',
             'e: constant 50000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)

#from TruthExamples.TruthExamplesConf import DumpMC
#topSeq += DumpMC()

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add the G4 sim to the alg sequence after the generator
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
