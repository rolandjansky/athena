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
# if not 'vp1Fitter' in dir(): vp1Fitter=False
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
if not 'vp1Batch' in dir(): vp1Batch=False
if not 'vp1BatchAllEvents' in dir(): vp1BatchAllEvents=False
if not 'vp1BatchNEvents' in dir(): vp1BatchNEvents=0
if not 'vp1BatchOutFolder' in dir(): vp1BatchOutFolder=""
if not 'vp1BatchRandomConfiguration' in dir(): vp1BatchRandomConfiguration=False
if not 'vp1LarHvData' in dir(): vp1LarHvData=False
# if not 'vp1FullToroids' in dir(): vp1FullToroids=False
if not 'vp1CruiseTime' in dir(): vp1CruiseTime=0
if not 'vp1NoSortDBReplicas' in dir(): vp1NoSortDBReplicas=False
if not 'vp1FilterEvents' in dir(): vp1FilterEvents=""
if not 'vp1NoGui' in dir(): vp1NoGui=False
if not 'vp1SpacePoints' in dir(): vp1SpacePoints=False
if not 'vp1Cavern' in dir(): vp1Cavern=False
if not 'vp1NoAutoConf' in dir(): vp1NoAutoConf=False
if not 'vp1Trig' in dir(): vp1Trig=False
if not 'vp1NSW' in dir(): vp1NSW=False

def vp1CfgErr(s): print "VP1 CONFIGURATION ERROR: %s" % s

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

if (vp1NSW and not vp1Muon):
    vp1CfgErr("Muon New Small Wheel (NSW) turned on, but no Muon geometry. Disabling NSW.")
    vp1NSW=False




print "*** VP1 NOTE *** setting COIN_GLXGLUE env vars to make screenshots working remotely..."
print "*** VP1 NOTE *** COIN_GLXGLUE_NO_GLX13_PBUFFERS=1 - " + "COIN_GLXGLUE_NO_PBUFFERS=1"
os.putenv("COIN_GLXGLUE_NO_GLX13_PBUFFERS","1")
os.putenv("COIN_GLXGLUE_NO_PBUFFERS","1")



#Set VP1ATLREL integer so we can do e.g. "if VP1ATLREL>=160002:"
include('VP1Algs/vp1FragmentConstructVP1ATLREL.py')

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

    # Set geometry version
    if (not "DetDescrVersion" in dir()):
        #DetDescrVersion = "ATLAS-GEO-20-00-01" # old
        DetDescrVersion = "ATLAS-R2-2015-03-01-00" # for the new Rel. 21
    globalflags.DetDescrVersion = DetDescrVersion
    
    # Set conditions tag
    if not 'vp1GlobCond' in dir():
        vp1GlobCond="OFLCOND-SDR-BS7T-05-14"
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(vp1GlobCond)


    ### NEW FOR REL. >= 22

    # Conditions Service for reading conditions data in serial and MT Athena
    from IOVSvc.IOVSvcConf import CondSvc
    svcMgr += CondSvc()

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    # Conditions data access infrastructure for serial and MT Athena
    from IOVSvc.IOVSvcConf import CondInputLoader
    condSeq += CondInputLoader( "CondInputLoader")

    import StoreGate.StoreGateConf as StoreGateConf
    svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

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
            DetDescrVersion = "ATLAS-GEO-10-00-02"

        globalflags.DetDescrVersion = DetDescrVersion
            
        # Set conditions tag
        if not 'vp1GlobCond' in dir():
            if (vp1Mc):
                vp1GlobCond="OFLCOND-SIM-BS7T-02"
            else:
                vp1GlobCond="COMCOND-BLKPST-004-01"

        from IOVDbSvc.CondDB import conddb
        conddb.setGlobalTag(vp1GlobCond)


        ### NEW FOR REL. >= 22

        # Conditions Service for reading conditions data in serial and MT Athena
        from IOVSvc.IOVSvcConf import CondSvc
        svcMgr += CondSvc()

        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        # Conditions data access infrastructure for serial and MT Athena
        from IOVSvc.IOVSvcConf import CondInputLoader
        condSeq += CondInputLoader( "CondInputLoader")

        import StoreGate.StoreGateConf as StoreGateConf
        svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

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


        ### NEW FOR REL. >= 22

        # Conditions Service for reading conditions data in serial and MT Athena
        from IOVSvc.IOVSvcConf import CondSvc
        svcMgr += CondSvc()

        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        # Conditions data access infrastructure for serial and MT Athena
        from IOVSvc.IOVSvcConf import CondInputLoader
        condSeq += CondInputLoader( "CondInputLoader")

        import StoreGate.StoreGateConf as StoreGateConf
        svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

if vp1FilterEvents:
    import VP1Algs.VP1EvtFilter
    vp1FilterEvents = VP1Algs.VP1EvtFilter.parseFilterString(vp1FilterEvents)
    VP1Algs.VP1EvtFilter.installEventFilter(vp1FilterEvents)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


#BATCH-MODE
# If "vp1Batch" is TRUE, then set the corresponding env var. 
# The GUI of VP1 will not be shown, but the config file will be taken
# and in the end a render of the 3D window will be saved as PNG file.
#
# Moreover, if "vp1BatchAllEvents" is TRUE, then all the events 
# in the provided data file will be processed with the same configuration
# file provided by the user.
# 
if vp1Batch:
    os.putenv("VP1_BATCHMODE","1")
if vp1BatchAllEvents:
    os.putenv("VP1_BATCHMODE_ALLEVENTS","1")
if vp1BatchNEvents > 0:
    os.putenv("VP1_BATCHMODE_NEVENTS", str(vp1BatchNEvents) )
if (vp1BatchOutFolder != ""):
    os.putenv("VP1_BATCHMODE_OUT_FOLDER", vp1BatchOutFolder)
if vp1BatchRandomConfiguration:
    os.putenv("VP1_BATCHMODE_RANDOMCONFIG","1")



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
if (vp1NSW): 
    DetFlags.Micromegas_setOn()
    DetFlags.sTGC_setOn()
    
    
DetFlags.Print()

# --- GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

if vp1Cavern:
  from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
  svcMgr.GeoModelSvc.DetectorTools += [ CavernInfraDetectorTool() ]

# --- AGDD2Geo
# Switch it on if
#  - Muon is ON
#  - Major geometry version is greater than 10
if (vp1Muon):
    if (vp1NSW): 
      #DetDescrVersion="ATLAS-GEO-21-00-01"
      # include('MuonGeoModelTest/NSWGeoSetup.py')
      from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
      GeoModelSvc = GeoModelSvc()
      GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.08.01-NSW"
    
      from MuonAGDD.MuonAGDDConf import NSWAGDDTool
      nTool=NSWAGDDTool('NewSmallWheel')
      nTool.ReadAGDD=False
      nTool.XMLFiles += ["stations.v2.03.xml"]
      nTool.DefaultDetector="Muon"
      nTool.Locked=False
      nTool.Volumes += ["NewSmallWheel"]
      ToolSvc+=nTool
    
      from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
      Agdd2Geo=AGDDtoGeoSvc()
      Agdd2Geo.Builders += ["NSWAGDDTool/NewSmallWheel"]
      theApp.CreateSvc += ["AGDDtoGeoSvc"]
      svcMgr += Agdd2Geo
    
    from AtlasGeoModel import Agdd2Geo
    
    # if(vp1FullToroids or vp1NSW):
    #     from AtlasGeoModel import Agdd2Geo
    # else:
    #     from AthenaCommon.GlobalFlags import globalflags
    #     DDV = globalflags.DetDescrVersion()
    #     if(DDV.__contains__('ATLAS-GEO')):
    #         seconddash = DDV.find('-',8);
    #         majorddversstr = DDV[seconddash+1:seconddash+3];
    #         majorddversint = int(majorddversstr);
    #         if(majorddversint>10):
    #             # Additional material in the muon system
    #             from AGDD2Geo.AGDD2GeoConf import AGDD2GeoSvc
    #             AGDD2GeoSvc=AGDD2GeoSvc()
    #             AGDD2GeoSvc.PrintSections=False
    #             AGDD2GeoSvc.Sections += ["ATLAS Feet"]
    #             AGDD2GeoSvc.Sections += ["Atlas Barrel Toroidal Magnets"]
    #             AGDD2GeoSvc.Sections += ["Atlas ECT Toroidal Magnets"]
    #             AGDD2GeoSvc.DisableSections = False
    #             AGDD2GeoSvc.Locked = False
    #             theApp.CreateSvc += ["AGDD2GeoSvc"]
    #             svcMgr += AGDD2GeoSvc

#MagneticField:
#include("BFieldAth/BFieldAth_jobOptions.py") # OLD
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
    
#watch LAr HV:
if ( vp1LarHvData ):
    from time import time
    svcMgr.AthenaSealSvc.CheckDictionary = TRUE
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

    from TrkExEngine.AtlasExtrapolationEngine import AtlasExtrapolationEngine
    ExtrapolationEngine = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas', ToolOutputLevel=INFO)
    ToolSvc += ExtrapolationEngine

    # Previous config from SetupFitters.py
    # from TrkExTools.TrkExToolsConf import Trk__Extrapolator as Extrapolator
    # VP1Extrapolator = Extrapolator(name='VP1Extrapolator',
    #                                Navigator = VP1Navigator,
    #                                MaterialEffectsUpdators = [ VP1MaterialUpdator ],
    #                                Propagators = [ VP1Propagator ] )

    ToolSvc += VP1Extrapolator
    #We should append to variable instead:
    os.putenv("VP1_JOBCFG_EXTRA_VP1_EXTRAPOLATORS","Trk::Extrapolator/"+VP1ExtraPolatorName)

#Fitter:
# if vp1Fitter and vp1Extrapolator and (vp1ID or vp1Muon):
#     VP1TrkInitializer.ForceFitterTools = True
#     os.putenv("VP1_DEVEL_ENABLEREFIT","1")
#     ##########################################################
#     # The Extrapolator
#     include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
#
#     from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
#     VP1Propagator = Propagator(name = 'VP1Propagator')
#     ToolSvc += VP1Propagator
#
#     # the Navigator has to get this one
#     from TrkExTools.TrkExToolsConf import Trk__Navigator
#     VP1Navigator = Trk__Navigator(name = 'VP1Navigator')
#     VP1Navigator.TrackingGeometrySvc = svcMgr.AtlasTrackingGeometrySvc
#     ToolSvc += VP1Navigator
#
#     from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MatUpdator
#     VP1MaterialUpdator = MatUpdator(name='VP1MaterialEffectsUpdator')
#     ToolSvc += VP1MaterialUpdator
#
#     from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
#     VP1Updator = Trk__KalmanUpdator_xk(name = 'VP1Updator')
#     ToolSvc += VP1Updator
#
#     from TrkDynamicNoiseAdjustor.TrkDynamicNoiseAdjustorConf import Trk__InDetDynamicNoiseAdjustment
#     VP1DNAdjustor = Trk__InDetDynamicNoiseAdjustment(name    = 'VP1DNAdjustor')
#                                                                #yminmax = 100.0)
#     ToolSvc += VP1DNAdjustor
#     from TrkKalmanFitter.TrkKalmanFitterConf import Trk__ForwardKalmanFitter as PublicFKF
#     VP1FKF = PublicFKF(name                  = 'VP1FKF',
#                         StateChi2PerNDFPreCut = 25.0)
#     ToolSvc += VP1FKF
#
#     from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanSmoother as PublicBKS
#     VP1BKS = PublicBKS(name                        = 'VP1BKS',
#                       InitialCovarianceSeedFactor = 200.)
#     ToolSvc += VP1BKS
#
#     from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanOutlierLogic as PublicKOL
#     VP1KOL = PublicKOL(name               = 'VP1KOL',
#                       StateChi2PerNDFCut = 12.5)
#     ToolSvc += VP1KOL
#
#     #FIXME! Only do this for Muons?
#     from MuonRecExample import MuonRecTools
#     MdtTubeHitOnTrackCreator      = MuonRecTools.getPublicTool("MdtTubeHitOnTrackCreator")
#
#     from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
#     VP1RotCreator = Trk__RIO_OnTrackCreator(name = 'VP1RotCreator',
#        ToolMuonDriftCircle = MdtTubeHitOnTrackCreator ,
#                                                  Mode = 'all')
#     ToolSvc += VP1RotCreator
#     print VP1RotCreator
#     print MdtTubeHitOnTrackCreator
#
#     from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter as ConfiguredKalmanFitter
#     VP1KalmanFitter = ConfiguredKalmanFitter(name                           = 'VP1KalmanFitter',
#                                              ExtrapolatorHandle             = VP1Extrapolator,
#                                              RIO_OnTrackCreatorHandle       = VP1RotCreator,
#                                              MeasurementUpdatorHandle       = VP1Updator,
#                                              ForwardKalmanFitterHandle      = VP1FKF,
#                                              KalmanSmootherHandle           = VP1BKS,
#                                              KalmanOutlierLogicHandle       = VP1KOL,
#                                              DynamicNoiseAdjustorHandle     = None,
#                                              AlignableSurfaceProviderHandle = None)
#
#     ToolSvc += VP1KalmanFitter
#
#     print VP1KalmanFitter
#     os.putenv("VP1_JOBCFG_EXTRA_VP1_FITTERS",VP1KalmanFitter.name())
#
#
#     VP1KalmanFitterDNA = ConfiguredKalmanFitter(name                        = 'VP1KalmanFitterDNA',
#                                              ExtrapolatorHandle             = VP1Extrapolator,
#                                              RIO_OnTrackCreatorHandle       = VP1RotCreator,
#                                              MeasurementUpdatorHandle       = VP1Updator,
#                                              ForwardKalmanFitterHandle      = VP1FKF,
#                                              KalmanSmootherHandle           = VP1BKS,
#                                              KalmanOutlierLogicHandle       = VP1KOL,
#                                              DynamicNoiseAdjustorHandle     = VP1DNAdjustor,
#                                              AlignableSurfaceProviderHandle = None)
#
#     ToolSvc += VP1KalmanFitterDNA
#     os.putenv("VP1_JOBCFG_EXTRA_VP1_FITTERS",VP1KalmanFitterDNA.name())
#
#
#     from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
#     VP1GlobalChi2Fitter = Trk__GlobalChi2Fitter(name               = 'VP1GlobalChi2Fitter',
#                                              ExtrapolationTool     = VP1Extrapolator,
#                                              NavigatorTool         = VP1Navigator,
#                                              PropagatorTool        = VP1Propagator,
#                                              RotCreatorTool        = VP1RotCreator,
#                                              MeasurementUpdateTool = VP1Updator,
#                                              StraightLine          = False,
#                                              OutlierCut            = 3.0,
#                                              SignedDriftRadius     = True,
#                                              RecalculateDerivatives= True
#                                              )
#     print VP1GlobalChi2Fitter
#     ToolSvc += VP1GlobalChi2Fitter
#
#     VP1GlobalChi2Fitter.OutputLevel=DEBUG
#
#     os.putenv("VP1_JOBCFG_EXTRA_VP1_FITTERS","Trk::KalmanFitter/"+VP1KalmanFitter.name()+";"+"Trk::KalmanFitter/"+VP1KalmanFitterDNA.name()+";"+"Trk::GlobalChi2Fitter/"+VP1GlobalChi2Fitter.name())

#On a machine where the hostname does not indicate domain, pool will
#fail if trying to find nearest replica. In any case, dblookup.xml
#needs to be patched when running on real data:
include('VP1Algs/vp1FragmentFixDBReplica.py')

if vp1NoSortDBReplicas:
    PoolSvc = Service( "PoolSvc" )
    PoolSvc.SortReplicas = False


#Finally, the VP1 algorithm itself:
from VP1Algs.VP1AlgsConf import VP1Alg
topSequence += VP1Alg()

VP1Alg.NoGui=vp1NoGui

if vp1CruiseTime > 0:
    VP1Alg.InitialCruiseMode = "EVENT"
    VP1Alg.InitialCruiseModePeriod = vp1CruiseTime

VP1Alg.InitiallyLoadedVP1Files = vp1CfgFiles
if (vp1Multinp):
    VP1Alg.MultipleFilesON = True
    VP1Alg.MFSourceDir = vp1Multinpsrc
    VP1Alg.MFLocalCopyDir = vp1Multinpcpy
    VP1Alg.MFAvailableLocalInputDirectories = vp1MultiAvailableSrcDirs

topSequence.TimeOut=0
