#==============================================================
#
# Job options file for Geant4 Simulations
#
# Standalone Tile TB during 2000-2003 
#
#==============================================================

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 4
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

#--- Number of events to be processed (default is 10) ---------
if not 'EvtMax' in dir():
    EvtMax = 10

#Set doEtaScan to True if you want to run at fixed eta
#Otherwise 90 degrees scan will be processed
if not 'doEtaScan' in dir():
    doEtaScan = True

if doEtaScan:
   if not 'Eta' in dir():
     Eta = 0.35
else:
   if not 'Z' in dir(): 
     Z=2795.0

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
#simFlags.SimLayout='tb_Tile2000_2003_2B2EB'
# TileCal standalone setup with 3 barrels
#simFlags.SimLayout='tb_Tile2000_2003_3B'
# TileCal standalone setup with 5 barrels
simFlags.SimLayout='tb_Tile2000_2003_5B'

simFlags.load_tbtile_flags()

#Set eta-projective scan or 90 degrees configuration
if doEtaScan:
    simFlags.Eta=Eta
else:
    simFlags.Theta=90
    simFlags.Z=Z

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
pg.sampler.pid = 11
pg.sampler.pos = PG.PosSampler(x=-27500, y=[-10,15], z=[-15,-15], t=-27500)
pg.sampler.mom = PG.EEtaMPhiSampler(energy=20000, eta=0, phi=0)
topSeq += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")

#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================

## Use verbose G4 tracking
#def use_verbose_tracking():
#    from G4AtlasApps import AtlasG4Eng
#    AtlasG4Eng.G4Eng.gbl.G4Commands().tracking.verbose(1)
#simFlags.InitFunctions.add_function("postInit", use_verbose_tracking)

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

#--- End of simulation part ---------------

#--- Tile hit vektor + MC truth info ----

#Store everything in CBNTAA
CBNTAthenaAware = True

if not hasattr(svcMgr,"THistSvc"):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr+=THistSvc()
svcMgr.THistSvc.Output += ["AANT DATAFILE='tiletb_aa.root' OPT='RECREATE'" ]

from AnalysisTools.AnalysisToolsConf import AANTupleStream
AANTupleStream = AANTupleStream()
topSeq += AANTupleStream
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = "tiletb_aa.root"
AANTupleStream.ExistDataHeader = False

include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

# truth information
from CBNT_Truth.CBNT_TruthConf import CBNTAA_Truth
CBNT_Truth = CBNTAA_Truth()
CBNT_AthenaAware += CBNT_Truth
CBNT_Truth.Members += [ "CBNT_TruthSelector/All" ]

from TileRec.TileRecConf import CBNTAA_TileHitVec
CBNTAA_TileHitVec = CBNTAA_TileHitVec()
CBNT_AthenaAware += CBNTAA_TileHitVec

##theApp.HistogramPersistency = "ROOT"
