#/*
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#*/

if not 'vp1InputFiles' in dir(): vp1InputFiles = []
if not 'vp1CfgFiles' in dir(): vp1CfgFiles = []
if not 'vp1ID' in dir(): vp1ID=True
if not 'vp1Calo' in dir(): vp1Calo=True
if not 'vp1Muon' in dir(): vp1Muon=True
if not 'vp1LUCID' in dir(): vp1LUCID=False
if not 'vp1ALFA' in dir(): vp1ALFA=False
if not 'vp1ForwardRegion' in dir(): vp1ForwardRegion=False
if not 'vp1ZDC' in dir(): vp1ZDC=False
if not 'vp1Extrapolator' in dir(): vp1Extrapolator=True
if not 'vp1Fitter' in dir(): vp1Fitter=False
if not 'vp1Fatras' in dir(): vp1Fatras=False
if not 'vp1FatrasMuon' in dir(): vp1FatrasMuon=(vp1Fatras and vp1Muon)
if not 'vp1FatrasCalo' in dir(): vp1FatrasCalo=(vp1Fatras and vp1Calo)
if not 'vp1FatrasTruthKey' in dir(): vp1FatrasTruthKey=""
if not 'vp1Online' in dir(): vp1Online=False
if not 'vp1Mc' in dir(): vp1Mc=False
if not 'vp1Multinp' in dir(): vp1Multinp=False
if not 'vp1Multinpsrc' in dir(): vp1Multinpsrc=""
if not 'vp1Multinpcpy' in dir(): vp1Multinpcpy=""
if not 'vp1MultiAvailableSrcDirs' in dir(): vp1MultiAvailableSrcDirs = []
if not 'vp1TrigDecXML' in dir(): vp1TrigDecXML=""
if not 'vp1LarHvData' in dir(): vp1LarHvData=False
# if not 'vp1FullToroids' in dir(): vp1FullToroids=False
if not 'vp1CruiseTime' in dir(): vp1CruiseTime=0
if not 'vp1NoSortDBReplicas' in dir(): vp1NoSortDBReplicas=False
if not 'vp1FilterEvents' in dir(): vp1FilterEvents=""
if not 'vp1NoGui' in dir(): vp1NoGui=False
if not 'vp1SpacePoints' in dir(): vp1SpacePoints=False
if not 'vp1Cavern' in dir(): vp1Cavern=False
if not 'vp1ToyDetector' in dir(): vp1ToyDetector=False
if not 'vp1GeoModelStats' in dir(): vp1GeoModelStats=False
if not 'vp1NoAutoConf' in dir(): vp1NoAutoConf=False
if not 'vp1Trig' in dir(): vp1Trig=False
if not 'vp1CustomGeometry' in dir(): vp1CustomGeometry=False
if not 'vp1SLHC' in dir(): vp1SLHC=False
if not 'vp1MuonAGDDFiles' in dir(): vp1MuonAGDDFiles=[]
if not 'vp1MuonAGDD2GeoSwitches' in dir(): vp1MuonAGDD2GeoSwitches=[]
if not 'vp1NSWAGDDFiles' in dir(): vp1NSWAGDDFiles=[]
if not 'vp1MuonLayout' in dir(): vp1MuonLayout=""

def vp1CfgErr(s): print ("VP1 CONFIGURATION ERROR: %s" % s)

if (vp1Fatras and not vp1ID):
    vp1CfgErr("Fatras can not be enabled without inner detector. Turning off Fatras.")
    vp1Fatras=False
if (vp1FatrasMuon and not vp1Fatras):
    vp1CfgErr("FatrasMuon can not be enabled without Fatras enabled. Turning off FatrasMuon.")
    vp1FatrasMuon=False
if (vp1FatrasMuon and not vp1Muon):
    vp1CfgErr("FatrasMuon can not be enabled without muon systems. Turning off FatrasMuon.")
    vp1FatrasMuon=False
if (vp1FatrasCalo and not vp1Fatras):
    vp1CfgErr("FatrasCalo can not be enabled without Fatras enabled. Turning off FatrasCalo.")
    vp1FatrasCalo=False
if (vp1FatrasCalo and not vp1Calo):
    vp1CfgErr("FatrasCalo can not be enabled without calo systems. Turning off FatrasCalo.")
    vp1FatrasCalo=False
if ( vp1FatrasTruthKey != "" and not vp1Fatras ):
    vp1CfgErr("FatrasTruthKey set but Fatras not enabled. Unsetting FatrasTruthKey.")
    vp1FatrasTruthKey=""

#Set VP1ATLREL integer so we can do e.g. "if VP1ATLREL>=160002:"
include('DumpGeo/vp1FragmentConstructVP1ATLREL.py')

#Disable FPE checks :-(
from RecExConfig.RecFlags import rec
rec.doFloatingPointException.set_Value_and_Lock(False)

if (vp1InputFiles == []):
    # Create an event selector:
    import AthenaCommon.AtlasUnixGeneratorJob
    # Configure GlobalFlags
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.DetGeo.set_Value_and_Lock('atlas')
    globalflags.DataSource.set_Value_and_Lock('geant4')

    # # Set geometry version
    if (not "DetDescrVersion" in dir()):
        DetDescrVersion = "ATLAS-R2-2016-01-00-01" # "ATLAS-R2-2015-03-01-00" for Rel. 21
        os.environ["DUMPGEODETDESCRTAGDEFAULT"] = "1" # save to an env var, for later use in GeoModelStandalone/GeoExporter
        os.environ["DUMPGEODETDESCRTAG"] = DetDescrVersion # save to an env var, for later use in GeoModelStandalone/GeoExporter
    globalflags.DetDescrVersion = DetDescrVersion

    # Set conditions tag
    if not 'vp1GlobCond' in dir():
        vp1GlobCond="OFLCOND-SDR-BS7T-05-14"
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(vp1GlobCond)
else:
    if(vp1NoAutoConf):
        # AutoConfiguration disabled
        import AthenaCommon.AtlasUnixStandardJob

        if(vp1Online):
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            athenaCommonFlags.isOnline = True

        from AthenaCommon.GlobalFlags import globalflags
        globalflags.DetGeo.set_Value_and_Lock('atlas')
        if(vp1Mc):
            globalflags.DataSource.set_Value_and_Lock('geant4')
        else:
            globalflags.DataSource.set_Value_and_Lock('data')
        globalflags.InputFormat.set_Value_and_Lock('pool')
        globalflags.Luminosity.set_Value_and_Lock('zero')
        from AthenaCommon.BeamFlags import jobproperties
        jobproperties.Beam.beamType="collisions"

        # Set geometry version
        if (not "DetDescrVersion" in dir()):
            DetDescrVersion = "ATLAS-R2-2016-01-00-01" # "ATLAS-R2-2015-03-01-00" for Rel. 21

        globalflags.DetDescrVersion = DetDescrVersion

        # Set conditions tag
        if not 'vp1GlobCond' in dir():
            if (vp1Mc):
                vp1GlobCond="OFLCOND-SIM-BS7T-02"
            else:
                vp1GlobCond="COMCOND-BLKPST-004-01"

        from IOVDbSvc.CondDB import conddb
        conddb.setGlobalTag(vp1GlobCond)
    else:
        # AutoConfiguration enabled
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        athenaCommonFlags.FilesInput=vp1InputFiles
        from RecExConfig.RecFlags import rec
        rec.AutoConfiguration=['everything']
        rec.doAOD.set_Value_and_Lock(False)
        rec.doESD.set_Value_and_Lock(False)

        # Override geometry tag from command line
        if("DetDescrVersion" in dir()):
            from AthenaCommon.GlobalFlags import globalflags
            globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)

        include('RecExCond/RecExCommon_flags.py')
        theApp.EvtMax = -1

        # Override global conditions tag from command line
        if('vp1GlobCond' in dir()):
            from IOVDbSvc.CondDB import conddb
            conddb.setGlobalTag(vp1GlobCond)

if vp1FilterEvents:
    import DumpGeo.VP1EvtFilter
    vp1FilterEvents = DumpGeo.VP1EvtFilter.parseFilterString(vp1FilterEvents)
    DumpGeo.VP1EvtFilter.installEventFilter(vp1FilterEvents)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#Detector setup:
from AthenaCommon.DetFlags import DetFlags
if (vp1ID): DetFlags.ID_setOn()
else:       DetFlags.ID_setOff()
if (vp1Calo): DetFlags.Calo_setOn()
else:         DetFlags.Calo_setOff()
if (vp1Muon): DetFlags.Muon_setOn()
else:         DetFlags.Muon_setOff()
if (vp1LUCID): DetFlags.Lucid_setOn()
else:          DetFlags.Lucid_setOff()
if (vp1ALFA): DetFlags.ALFA_setOn()
else:          DetFlags.ALFA_setOff()
if (vp1ForwardRegion): DetFlags.FwdRegion_setOn()
else:          DetFlags.FwdRegion_setOff()
if (vp1ZDC): DetFlags.ZDC_setOn()
else:          DetFlags.ZDC_setOff()
DetFlags.Print()
if (vp1CustomGeometry):
    print ("Configuring Custom geometry.")
if (vp1SLHC):
  print ("Setting up SLHC configuration")
  rec.doTrigger.set_Value_and_Lock(False)
  from AthenaCommon.GlobalFlags import globalflags
  from AthenaCommon.GlobalFlags import jobproperties
  from InDetRecExample.InDetJobProperties import InDetFlags

  #include("InDetSLHC_Example/preInclude.SLHC.py")
  #include("InDetSLHC_Example/preInclude.NoTRT.py")
  #include("InDetSLHC_Example/preInclude.SLHC_Rec.py")
  # The above don't work for dev (20.9.0) so copied modified versions below:
  from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
  SLHC_Flags.SLHC_Version = ''
  DetFlags.ID_setOn()
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOff()
  DetFlags.makeRIO.TRT_setOff()
  DetFlags.Calo_setOn()
  DetFlags.Muon_setOn()
  DetFlags.Truth_setOn()
  from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
  TrkDetFlags.SLHC_Geometry                   = True
  TrkDetFlags.MagneticFieldCallbackEnforced   = False
  TrkDetFlags.TRT_BuildStrawLayers            = False
  TrkDetFlags.MaterialSource = 'None'

if vp1Muon and vp1MuonLayout!="":
    print ("*** DumpGeo NOTE *** You specified custom vp1MuonLayout, using %s as muon geometry"%vp1MuonLayout)
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()
    GeoModelSvc.MuonVersionOverride=vp1MuonLayout

# --- GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

if vp1GeoModelStats:
  print ("printing GeoModel stats to file (Svc.StatisticsToFile=TRUE)...")
  svcMgr.GeoModelSvc.StatisticsToFile = True

if vp1Cavern:
  print ("vp1.py - Initializing the visualization of the Cavern Infrastructure...")
  from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
  svcMgr.GeoModelSvc.DetectorTools += [ CavernInfraDetectorTool() ]

if vp1ToyDetector:
  print ("vp1.py - Initializing the visualization of the GeoModelExamples 'ToyDetector'...")
  from GeoModelExamples.GeoModelExamplesConf import ToyDetectorTool
  svcMgr.GeoModelSvc.DetectorTools += [ ToyDetectorTool() ]




# --- AGDD2Geo
# Switch it on if
#  - Muon is ON
#  - Major geometry version is greater than 10
if (vp1Muon):

    from AtlasGeoModel import Agdd2Geo

    if len(vp1MuonAGDDFiles)>0:
        print ("*** DumpGeo NOTE *** You specified custom vp1MuonAGDDFiles, configuring MuonAGDDTool to read MuonAGDD information from custom file(s) '%s' instead from built-in geometry"%(', '.join(vp1MuonAGDDFiles)))
        if hasattr(svcMgr,"AGDDtoGeoSvc"):
            for b in getattr(svcMgr,"AGDDtoGeoSvc").Builders:
                if b.name()=="MuonSpectrometer":
                    b.ReadAGDD=False
                    b.XMLFiles=vp1MuonAGDDFiles
                    if len(vp1MuonAGDD2GeoSwitches)>0:
                        print ("*** DumpGeo NOTE *** You specified custom vp1MuonAGDD2GeoSwitches, configuring MuonAGDDTool to build volumes: '%s'"%(', '.join(vp1MuonAGDD2GeoSwitches)))
                        b.Volumes=vp1MuonAGDD2GeoSwitches
                    else:
                        # the default AGDD2GeoSwitches for Run2
                        b.Volumes=["ECT_Toroids",
                                   "BAR_Toroid",
                                   "Feet",
                                   "RailAssembly",
                                   "JFSH_Shield",
                                   "JDSH_Shield",
                                   "JTSH_Shield",
                                   "pp2",
                                   "MBAP_AccessPlatform",
                                   "MBWH_BigWheels",
                                   "SADL_CalorimeterSaddle",
                                   "TBWH_BigWheels",
                                   "TGC3_BigWheels",
                                   "TGC1_BigWheels",
                                   "MDTRail",
                                   "servicesAtZ0",
                                   "HFTruckRail",
                                   "RUN2_Services"]
    if len(vp1NSWAGDDFiles)>0:
        print ("*** DumpGeo NOTE *** You specified custom vp1NSWAGDDFiles, configuring NSWAGDDTool to read NSWAGDD information from custom file(s) '%s' instead from built-in geometry"%(', '.join(vp1NSWAGDDFiles)))
        if hasattr(svcMgr,"AGDDtoGeoSvc"):
            for b in getattr(svcMgr,"AGDDtoGeoSvc").Builders:
                if b.name()=="NewSmallWheel":
                    b.ReadAGDD=False
                    b.XMLFiles=vp1NSWAGDDFiles

#MagneticField:
import MagFieldServices.SetupField # NEW

if (vp1Fatras):
    from FatrasExample.FatrasJobProperties import FatrasFlags
    FatrasFlags.SingleTrackSimulation.set_Value(vp1InputFiles==[])
    from FastSimulationConfig.FastSimulationFlags import jobproperties
    jobproperties.FastSimulation.doFatrasID.set_Value(True)
    jobproperties.FastSimulation.doFatrasMuon.set_Value(vp1FatrasMuon)
    FatrasFlags.FastCaloSim.set_Value(vp1FatrasCalo)

if (vp1InputFiles != []):
    import AthenaPoolCnvSvc.ReadAthenaPool

    if (vp1ID):
        from InDetRecExample.InDetJobProperties import InDetFlags
        InDetFlags.loadTools               = (vp1Extrapolator or vp1SpacePoints)
        InDetFlags.preProcessing           = vp1SpacePoints
        InDetFlags.doPRDFormation          = False
        InDetFlags.doSpacePointFormation   = vp1SpacePoints
        InDetFlags.doNewTracking           = False
        InDetFlags.doLowPt                 = False
        InDetFlags.doLowBetaFinder         = False
        InDetFlags.doBackTracking          = False
        InDetFlags.doTRTStandalone         = False
        InDetFlags.doTrtSegments           = False
        InDetFlags.postProcessing          = False
        InDetFlags.doSlimming              = False
        InDetFlags.doVertexFinding         = False
        InDetFlags.doParticleCreation      = False
        InDetFlags.doConversions           = False
        InDetFlags.doSecVertexFinder       = False
        InDetFlags.doV0Finder              = False
        InDetFlags.doTrkNtuple             = False
        InDetFlags.doPixelTrkNtuple        = False
        InDetFlags.doSctTrkNtuple          = False
        InDetFlags.doTrtTrkNtuple          = False
        InDetFlags.doVtxNtuple             = False
        InDetFlags.doConvVtxNtuple         = False
        InDetFlags.doV0VtxNtuple           = False
        InDetFlags.doRefit                 = False

        InDetFlags.doStatistics            = False
        InDetFlags.useDCS                  = False
        include( "InDetRecExample/InDetRec_jobOptions.py" )

        if (vp1Fatras and vp1FatrasTruthKey!=""):
            from FatrasExample.FatrasKeys import FatrasKeyFlags
            FatrasKeyFlags.InputMcEventCollection.set_Value(vp1FatrasTruthKey)

    if (vp1Trig):
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        TriggerConfigGetter( "ReadPool" )

    if (vp1Calo):
        from LArConditionsCommon import LArAlignable

        #
        # Following is needed to display Tile Pulse shapes on ESD files
        #
        from TileConditions.TileInfoConfigurator import TileInfoConfigurator
        tileInfoConfigurator = TileInfoConfigurator()

    if (vp1Muon):
        include( "AmdcAth/AmdcAth_jobOptions.py" )

    include( "PartPropSvc/PartPropSvc.py" )

    if(vp1NoAutoConf):
        svcMgr.EventSelector.InputCollections=vp1InputFiles
    else:
        svcMgr.EventSelector.InputCollections=athenaCommonFlags.FilesInput()

    if 'skipEvents' in dir():
        svcMgr.EventSelector.SkipEvents=skipEvents
else:
    vp1Extrapolator = False

# do not need to run execute since layout is dumped during initialize
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 0

#watch LAr HV:
if ( vp1LarHvData ):
    from time import time
    svcMgr.EventSelector.InitialTimeStamp  = int (time())
    svcMgr.EventSelector.TimeStampInterval = 1
    from IOVDbSvc.CondDB import conddb
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16")
    conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8")
#------------

if (vp1Fatras):
    include( "FatrasExample/Fatras_jobOptions.py" )

if (vp1TrigDecXML!=""):
    include( "TrigConfigSvc/jobOptions_commonSetup.py" )
    include( "TrigConfigSvc/jobOptions_setupLVL1Svc.py" )
    include( "TrigConfigSvc/jobOptions_setupHLTSvc.py" )
    HLTConfigSvc.XMLMenuFile     = vp1TrigDecXML
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    ToolSvc += Trig__TrigDecisionTool("TrigDecisionTool")

if (vp1Calo):
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource() == 'data' :
        include ("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
    else:
        include ("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
        include ("LArConditionsCommon/LArIdMap_MC_jobOptions.py")

#Extrapolator:
if vp1Extrapolator and (vp1ID or vp1Muon):
    # Force tool initializations needed by the extrapolator
    from VP1TrkAuxAlgs.VP1TrkAuxAlgsConf import VP1TrkInitializer
    VP1TrkInitializer = VP1TrkInitializer()
    VP1TrkInitializer.ForceExtrapolatorTools = True
    topSequence += VP1TrkInitializer

    if not vp1Fatras and vp1Mc:
        include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')

    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    VP1ExtraPolatorName='VP1Extrapolator';
    VP1Extrapolator = AtlasExtrapolator(name=VP1ExtraPolatorName)

    # Previous config from SetupFitters.py
    # from TrkExTools.TrkExToolsConf import Trk__Extrapolator as Extrapolator
    # VP1Extrapolator = Extrapolator(name='VP1Extrapolator',
    #                                Navigator = VP1Navigator,
    #                                MaterialEffectsUpdators = [ VP1MaterialUpdator ],
    #                                Propagators = [ VP1Propagator ] )

    ToolSvc += VP1Extrapolator
    print (VP1Extrapolator)
    #We should append to variable instead:
    os.putenv("VP1_JOBCFG_EXTRA_VP1_EXTRAPOLATORS","Trk::Extrapolator/"+VP1ExtraPolatorName)

    from TrkExEngine.AtlasExtrapolationEngine import AtlasExtrapolationEngine
    ExtrapolationEngine = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas')
    ToolSvc += ExtrapolationEngine
    print (ExtrapolationEngine)

#Fitter:
if vp1Fitter and vp1Extrapolator and (vp1ID or vp1Muon):
    VP1TrkInitializer.ForceFitterTools = True
    os.putenv("VP1_DEVEL_ENABLEREFIT","1")
    ##########################################################
    # The Extrapolator
    include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')

    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
    VP1Propagator = Propagator(name = 'VP1Propagator')
    ToolSvc += VP1Propagator

    # the Navigator has to get this one
    from TrkExTools.TrkExToolsConf import Trk__Navigator
    VP1Navigator = Trk__Navigator(name = 'VP1Navigator')
    VP1Navigator.TrackingGeometrySvc = svcMgr.AtlasTrackingGeometrySvc
    ToolSvc += VP1Navigator

    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MatUpdator
    VP1MaterialUpdator = MatUpdator(name='VP1MaterialEffectsUpdator')
    ToolSvc += VP1MaterialUpdator

    from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
    VP1Updator = Trk__KalmanUpdator_xk(name = 'VP1Updator')
    ToolSvc += VP1Updator

    from TrkDynamicNoiseAdjustor.TrkDynamicNoiseAdjustorConf import Trk__InDetDynamicNoiseAdjustment
    VP1DNAdjustor = Trk__InDetDynamicNoiseAdjustment(name    = 'VP1DNAdjustor')
                                                               #yminmax = 100.0)
    ToolSvc += VP1DNAdjustor
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__ForwardKalmanFitter as PublicFKF
    VP1FKF = PublicFKF(name                  = 'VP1FKF',
                        StateChi2PerNDFPreCut = 25.0)
    ToolSvc += VP1FKF

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanSmoother as PublicBKS
    VP1BKS = PublicBKS(name                        = 'VP1BKS',
                      InitialCovarianceSeedFactor = 200.)
    ToolSvc += VP1BKS

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanOutlierLogic as PublicKOL
    VP1KOL = PublicKOL(name               = 'VP1KOL',
                      StateChi2PerNDFCut = 12.5)
    ToolSvc += VP1KOL

    #FIXME! Only do this for Muons?
    from MuonRecExample import MuonRecTools
    MdtTubeHitOnTrackCreator      = MuonRecTools.getPublicTool("MdtTubeHitOnTrackCreator")

    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    VP1RotCreator = Trk__RIO_OnTrackCreator(name = 'VP1RotCreator',
       ToolMuonDriftCircle = MdtTubeHitOnTrackCreator ,
                                                 Mode = 'all')
    ToolSvc += VP1RotCreator
    print (VP1RotCreator)
    print (MdtTubeHitOnTrackCreator)

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter as ConfiguredKalmanFitter
    VP1KalmanFitter = ConfiguredKalmanFitter(name                           = 'VP1KalmanFitter',
                                             ExtrapolatorHandle             = VP1Extrapolator,
                                             RIO_OnTrackCreatorHandle       = VP1RotCreator,
                                             MeasurementUpdatorHandle       = VP1Updator,
                                             ForwardKalmanFitterHandle      = VP1FKF,
                                             KalmanSmootherHandle           = VP1BKS,
                                             KalmanOutlierLogicHandle       = VP1KOL,
                                             DynamicNoiseAdjustorHandle     = None,
                                             AlignableSurfaceProviderHandle = None)

    ToolSvc += VP1KalmanFitter

    print (VP1KalmanFitter)
    os.putenv("VP1_JOBCFG_EXTRA_VP1_FITTERS",VP1KalmanFitter.name())


    VP1KalmanFitterDNA = ConfiguredKalmanFitter(name                        = 'VP1KalmanFitterDNA',
                                             ExtrapolatorHandle             = VP1Extrapolator,
                                             RIO_OnTrackCreatorHandle       = VP1RotCreator,
                                             MeasurementUpdatorHandle       = VP1Updator,
                                             ForwardKalmanFitterHandle      = VP1FKF,
                                             KalmanSmootherHandle           = VP1BKS,
                                             KalmanOutlierLogicHandle       = VP1KOL,
                                             DynamicNoiseAdjustorHandle     = VP1DNAdjustor,
                                             AlignableSurfaceProviderHandle = None)

    ToolSvc += VP1KalmanFitterDNA
    os.putenv("VP1_JOBCFG_EXTRA_VP1_FITTERS",VP1KalmanFitterDNA.name())


    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
    VP1GlobalChi2Fitter = Trk__GlobalChi2Fitter(name               = 'VP1GlobalChi2Fitter',
                                             ExtrapolationTool     = VP1Extrapolator,
                                             NavigatorTool         = VP1Navigator,
                                             PropagatorTool        = VP1Propagator,
                                             RotCreatorTool        = VP1RotCreator,
                                             MeasurementUpdateTool = VP1Updator,
                                             StraightLine          = False,
                                             OutlierCut            = 3.0,
                                             SignedDriftRadius     = True,
                                             RecalculateDerivatives= True
                                             )
    print (VP1GlobalChi2Fitter)
    ToolSvc += VP1GlobalChi2Fitter

    VP1GlobalChi2Fitter.OutputLevel=DEBUG

    os.putenv("VP1_JOBCFG_EXTRA_VP1_FITTERS","Trk::KalmanFitter/"+VP1KalmanFitter.name()+";"+"Trk::KalmanFitter/"+VP1KalmanFitterDNA.name()+";"+"Trk::GlobalChi2Fitter/"+VP1GlobalChi2Fitter.name())

#On a machine where the hostname does not indicate domain, pool will
#fail if trying to find nearest replica. In any case, dblookup.xml
#needs to be patched when running on real data:
include('DumpGeo/vp1FragmentFixDBReplica.py')

if vp1NoSortDBReplicas:
    PoolSvc = Service( "PoolSvc" )
    PoolSvc.SortReplicas = False


#Finally, the VP1 algorithm itself:
from DumpGeo.DumpGeoConf import DumpGeo
topSequence += DumpGeo()

DumpGeo.NoGui=vp1NoGui

if vp1CruiseTime > 0:
    DumpGeo.InitialCruiseMode = "EVENT"
    DumpGeo.InitialCruiseModePeriod = vp1CruiseTime

DumpGeo.InitiallyLoadedVP1Files = vp1CfgFiles
if (vp1Multinp):
    DumpGeo.MultipleFilesON = True
    DumpGeo.MFSourceDir = vp1Multinpsrc
    DumpGeo.MFLocalCopyDir = vp1Multinpcpy
    DumpGeo.MFAvailableLocalInputDirectories = vp1MultiAvailableSrcDirs

topSequence.TimeOut=0
