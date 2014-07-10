#==============================================================
#
# Job options file for Geant4 Simulations
#
# Standalone Tile TB during 2000-2003 
#
#==============================================================

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()


#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

#--- Number of events to be processed (default is 10) ---------
if not 'EvtMax' in dir():
    EvtMax = 10

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-CTB-01"

# - Select detectors 
DetFlags.Calo_setOn()
DetFlags.Tile_setOn()
# - MCTruth
DetFlags.simulate.Truth_setOn()
#
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput='Tile_MyOutputFile.root'
athenaCommonFlags.EvtMax=EvtMax


#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags

# TileCal standalone setup with 2 barrels and 2 ext.barrels on top
simFlags.SimLayout='tb_Tile2000_2003_2B2EB'
# TileCal standalone setup with 3 barrels
#simFlags.SimLayout='tb_Tile2000_2003_3B'
# TileCal standalone setup with 5 barrels
#simFlags.SimLayout='tb_Tile2000_2003_5B'

# Goes with a new version of G4AtlasApps (>= G4AtlasApps-00-04-08), for older releases comment this line!
simFlags.load_tbtile_flags()

# set eta only if you want eta-projective scan
simFlags.Eta=0.35
#
# put 5.625 or -5.625 if you want to rotate table to up/bottom module
#simFlags.Phi=5.625
#
# put non-zero value here if you want beam above (>0) or below (<0) center
#simFlags.Y=5.0

# these two values Theta and Z can be set instead of eta
# negative theta corresponds to positive eta values
#simFlags.Theta=-19.656205808169407
# Z coordinate is calculated along front face of module at R=2290
# this is the distance from center of the modules to the desired impact point
# positive value - impact point at positive eta side
#simFlags.Z=817.96448041135261
#
# for 90 degrees scans put theta=+/-90
# positive theta - beam enters from positive eta side (as defined in CTB setup!)
#simFlags.Theta=90
# Z coordinate is the distance from ATLAS center to the desired impact point
# sensitive part starts at Z=2300, ends at Z=2300+3*100+3*130+3*150+2*190=3820
#simFlags.Z=2550.0


# uncomment this for simulation with calibration hits
#simFlags.CalibrationRun = 'Tile'

# uncomment and modify any of options below to have non-standard simulation 
from TileSimUtils.TileSimInfoConfigurator import TileSimInfoConfigurator
tileSimInfoConfigurator=TileSimInfoConfigurator()
# tileSimInfoConfigurator.DeltaTHit = [ 1. ]
# tileSimInfoConfigurator.TimeCut = 200.5
# tileSimInfoConfigurator.TileTB = True
# tileSimInfoConfigurator.PlateToCell = True
# tileSimInfoConfigurator.DoExpAtt = False
# tileSimInfoConfigurator.DoBirk = True
# tileSimInfoConfigurator.DoTileRow = False
# tileSimInfoConfigurator.DoTOFCorrection = True
print tileSimInfoConfigurator

# avoid reading CaloTTMap from COOL
include.block ( "CaloConditions/CaloConditions_jobOptions.py" )


#--- Generator flags ------------------------------------------

#simFlags.PhysicsList.set_Value('QGSP_EMV')
simFlags.PhysicsList.set_Value('QGSP_BERT')
#simFlags.PhysicsList.set_Value('QGSP_BERT_EMV')

## Run ParticleGun
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 211
pg.sampler.pos = PG.PosSampler(x=-27500, y=[-10,15], z=[-15,-15], t=-27500)
pg.sampler.mom = PG.EEtaMPhiSampler(energy=50000, eta=0, phi=0)
topSeq += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")

#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================

#--- TB-Tile  setup description  -----------------------------

## Use verbose G4 tracking
#def use_verbose_tracking():
#    from G4AtlasApps import AtlasG4Eng
#    AtlasG4Eng.G4Eng.gbl.G4Commands().tracking.verbose(1)
#simFlags.InitFunctions.add_function("postInit", use_verbose_tracking)
            
## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

#--- End jobOptions_TileTB_Sim.py file  ---------------

