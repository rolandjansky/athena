###############################################################
## Job options file for Geant4 Tile subdetector simulations
#==============================================================

if not 'EvtMax' in dir():
    EvtMax = 10

if not 'File' in dir():
    File = 'HITS'

if not 'OutputFile' in dir():
    OutputFile = '%s.pool.root' % File

if not 'ConddbTag' in dir():
    ConddbTag = 'OFLCOND-MC21-SDR-RUN3-08'

if not 'DetDescrVersion' in dir():
    DetDescrVersion = 'ATLAS-R3S-2021-03-00-00'

if not 'TileVersionOverride' in dir():
#    TileVersionOverride = 'TileCal-GEO-12'
    TileVersionOverride = ''

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

## Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 10000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E200_eta0-60-10000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = OutputFile
athenaCommonFlags.EvtMax = EvtMax

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.LAr_setOff()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOff()

## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = ConddbTag

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.RunNumber = 410000

## Turns on calibration hits for Tile
if 'CalibrationRun' in dir():
    simFlags.CalibrationRun = 'Tile'

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
simFlags.SimLayout.set_On()

## Set a specific layout ATLAS-GEO tag:
simFlags.SimLayout = DetDescrVersion

## Set a specific Tile-Geo tag:
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if len(TileVersionOverride)>0:
    GeoModelSvc.TileVersionOverride = TileVersionOverride

#---- Needed for the U-shape (any value of Ushape > 0- means that tile size equal to size of master plate, for Ushape <=0 - size of the tile is like in old geometry )
if 'TileUshape' in dir():
    from AtlasGeoModel import TileGM
    GeoModelSvc.DetectorTools[ "TileDetectorTool" ].Ushape = TileUshape

#---- Special option to enable Cs tubes in simulation. Any value > 0 enables them
if 'TileCsTube' in dir():
    from AtlasGeoModel import TileGM
    GeoModelSvc.DetectorTools[ "TileDetectorTool" ].CsTube = TileCsTube

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_On()

## Set the LAr parameterization
#simFlags.LArParameterization = 2

## No magnetic field
#simFlags.MagneticField.set_Off()

## Keep GeoModel description in memory
#simFlags.ReleaseGeoModel.set_Off()

#--- Geant4 flags ---------------------------------------------

## Select G4 physics list or use the default one
#simFlags.PhysicsList.set_Value('QGSP_EMV')
#simFlags.PhysicsList.set_Value('QGSP_BERT_EMV')
#simFlags.PhysicsList.set_Value('QGSP_BERT')
#simFlags.PhysicsList.set_Value('FTFP_BERT')

## Register callback functions at various init stages
#def test_preInit():
#    printfunc ("CALLBACK AT PREINIT")
#def test_postInit():
#    printfunc ("CALLBACK AT POSTINIT")
#simFlags.InitFunctions.add_function("preInit", test_preInit)
#simFlags.InitFunctions.add_function("postInit", test_postInit)

## Change the field stepper or use verbose G4 tracking
#from G4AtlasApps import callbacks
#callbacks.use_simplerunge_stepper()
#callbacks.use_verbose_tracking()

## Use verbose G4 tracking
if 'VerboseTracking' in dir():
    simFlags.G4Commands+= ['/tracking/verbose 1']

#---- If GDML variable is defined, full geometery is dumped to GDML file
if 'GDML' in dir():
    include("G4DebuggingTools/VolumeDebugger_options.py")
    #simFlags.OptionalUserActionList.addAction('G4UA::VolumeDebuggerTool')
    simFlags.UserActionConfig.addConfig('G4UA::VolumeDebuggerTool',"OutputPath","./Tile.gdml")
    simFlags.UserActionConfig.addConfig('G4UA::VolumeDebuggerTool',"TargetVolume","CALO::CALO") #You can set this to whatever volume you like

#--- Generator flags ------------------------------------------

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()

## Use single particle generator
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")

if not 'PID' in dir():
    PID=211
if not 'E' in dir():
    E=100000
pg.sampler.pid = PID
pg.sampler.pos = PG.PosSampler(x=0, y=0, z=0, t=0)
pg.sampler.mom = PG.EEtaMPhiSampler(energy=E, eta=[-1.8,1.8], phi=[0,6.28318])

topSeq += pg

#--- Final step -----------------------------------------------

# This should only be used when evgen is run in the simulation step
include('G4AtlasApps/fragment.SimCopyWeights.py')

include("G4AtlasApps/G4Atlas.flat.configuration.py")

#from TruthExamples.TruthExamplesConf import DumpMC
#topSeq += DumpMC()

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add the G4 sim to the alg sequence after the generator
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
printfunc (topSeq)

## Set non-standard range cut
if 'RangeCut' in dir():
    svcMgr.ToolSvc.PhysicsListToolBase.GeneralCut=RangeCut

## uncomment and modify any of options below to have non-standard simulation 
SD = svcMgr.TileGeoG4SDCalc
#SD.TileTB=False
# SD.DeltaTHit = [ 50. ]
# SD.TimeCut = 200.5
# SD.PlateToCell = True
# SD.DoTileRow = False
# SD.DoTOFCorrection = True
# Birks' law
if 'DoBirk' in dir():
    SD.DoBirk = DoBirk
if 'TileUshape' in dir():
    SD.Ushape=TileUshape
printfunc (SD)

## VP1 algorithm for visualization
if 'VP1' in dir():
    from VP1Algs.VP1AlgsConf import VP1Alg
    topSeq += VP1Alg()

## Write Calibration Hit ID and hit coordinates into Ntuple
if 'CalibrationRun' in dir() and 'HitInfo' in dir():
    ## Root Ntuple output
    theApp.HistogramPersistency = "ROOT"
    svcMgr = theApp.serviceMgr()
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
    svcMgr.THistSvc.Output += [ "AANT DATAFILE='Hits.Ntup.root' OPT='RECREATE' " ]
    svcMgr.THistSvc.MaxFileSize = 32768
    #svcMgr.THistSvc.CompressionLevel = 5

    ## Tool to create TileCalibHitCnt Ntuple
    from AthenaCommon.AppMgr import ToolSvc
    from TileSimUtils.TileSimUtilsConf import TileCalibHitCntNtup
    theTileCalibHitCntNtup = TileCalibHitCntNtup()
    ToolSvc += theTileCalibHitCntNtup

