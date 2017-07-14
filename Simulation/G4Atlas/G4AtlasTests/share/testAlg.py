## Job options file for Geant4 simulations

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.simulate.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'atlasg4.hits.pool.root'
athenaCommonFlags.EvtMax = 10

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-RUN12-SDR-19"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On() # uses the default value

## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()
#simFlags.LArParameterization = 2
#simFlags.MagneticField.set_Off()

## Use single particle generator
simFlags.KinematicsMode = 'SingleParticle'
simFlags.ParticlePDG = '13'
simFlags.Energy = 10000
simFlags.ParticleGeneratorOrders = {
    'vertX:' : ' constant 0.0',
    'vertY:' :' constant 0.0',
    'vertZ:' : ' constant 0.0',
    't:' :' constant 0.0',
    'eta:' : ' flat -0.6 0.6',
    'phi:' : ' flat  0 6.28318',
    'pt:' : ' constant 5000' }

## Output printout level (DEBUG, INFO, WARNING, ERROR, FATAL)
messageSvc = Service("MessageSvc")
messageSvc.OutputLevel = WARNING

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["truth DATAFILE='truth.root' OPT='NEW'"];

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
job += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
job += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

from G4AtlasTests.G4AtlasTestsConf import G4TestAlg
job += G4TestAlg()
from AthenaCommon import CfgGetter
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TruthTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("PixelHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("SCT_HitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TrtHitsTestTool", checkType=True)]
job.job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMBHitsTestTool", checkType=True)]
job.job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMECHitsTestTool", checkType=True)]
job.job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("HECHitsTestTool", checkType=True)]
job.job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("FCALHitsTestTool", checkType=True)]
# include("G4UserActions/RadLengthIntegrator_options.py")
# include("G4UserActions/IntLengthIntegrator_options.py")
#simFlags.G4Commands += ['/tracking/verbose 1']
# AtlasG4Eng.G4Eng._ctrl.load("Stau")
