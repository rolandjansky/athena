## _PyG4AtlasComp.__init__
## If the random number service hasn't been set up already, do it now.
from G4AtlasApps.SimFlags import simFlags
simFlags.RandomSeedList.useDefaultSeeds()

## AtlasSimSkeleton._do_jobproperties
## Import extra flags if it hasn't already been done
from G4AtlasApps.SimFlags import simFlags
if "atlas_flags" not in simFlags.extra_flags:
    simFlags.load_atlas_flags()
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType() == "cosmics" and "cosmics_flags" not in simFlags.extra_flags:
    simFlags.load_cosmics_flags()

## Global flags needed by externals
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'geant4'
if jobproperties.Beam.beamType() == 'cosmics':
    globalflags.DetGeo = 'commis'
else:
    globalflags.DetGeo = 'atlas'

## At this point we can set the global job properties flag
globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()

from AthenaCommon.DetFlags import DetFlags
## Tidy up DBM DetFlags: temporary measure
DetFlags.DBM_setOff()

## Tidy up NSW DetFlags: temporary measure
DetFlags.sTGC_setOff()
DetFlags.Micromegas_setOff()
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
if ( hasattr(simFlags, 'SimulateNewSmallWheel') and simFlags.SimulateNewSmallWheel() ) or CommonGeometryFlags.Run()=="RUN3" :
    DetFlags.sTGC_setOn()
    DetFlags.Micromegas_setOn()

## Switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulateLVL1.all_setOff()
DetFlags.digitize.all_setOff()
if not simFlags.IsEventOverlayInputSim():
    DetFlags.overlay.all_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.writeRDOPool.all_setOff()

# Switch off GeoModel Release in the case of parameterization
if simFlags.LArParameterization.get_Value()>0 and simFlags.ReleaseGeoModel():
    simFlags.ReleaseGeoModel = False

## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
from AthenaCommon.AppMgr import ServiceMgr
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
ServiceMgr += IOVDbSvc()
if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
    raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
    ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

## Enable floating point exception handling
## FIXME! This seems to cause the jobs to crash in the FpeControlSvc, so commenting this out for now...
#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.RuntimeStrictness = 'abort'

if not simFlags.ISFRun:
    from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
    checkForSpecialConfigurationMetadata()

from AthenaCommon.JobProperties import jobproperties
DetFlags.Print()
jobproperties.print_JobProperties('tree&value')

# Lock the job properties if not running ISF.
if not simFlags.ISFRun:
    jobproperties.lock_JobProperties()

## AtlasSimSkeleton._do_external
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from Geo2G4.Geo2G4Conf import Geo2G4Svc
geo2G4Svc = Geo2G4Svc()
theApp.CreateSvc += ["Geo2G4Svc"]
ServiceMgr += geo2G4Svc
## Enable top transforms for the ATLAS geometry
geo2G4Svc.GetTopTransform = True

## GeoModel stuff
## TODO: Tidy imports etc.
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AthenaCommon.GlobalFlags import jobproperties
from AtlasGeoModel import SetGeometryVersion

## Forward Region Twiss files - needed before geometry setup!
from G4AtlasApps.SimFlags import simFlags
if simFlags.ForwardDetectors.statusOn:
    if DetFlags.geometry.FwdRegion_on():
        from AthenaCommon.CfgGetter import getPublicTool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += getPublicTool("ForwardRegionProperties")

from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SimEnvelopes
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
gms = GeoModelSvc()
## Cosmics GeoModel tweaks
if jobproperties.Beam.beamType() == 'cosmics' or \
   (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
    from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
    gms.DetectorTools += [ CavernInfraDetectorTool() ]
## Protects GeoModelSvc in the simulation from the AlignCallbacks
gms.AlignCallbacks = False
## Muon GeoModel tweaks
if DetFlags.Muon_on():
    ## Turn off caching in the muon system
    from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
    MuonDetectorTool = MuonDetectorTool()
    MuonDetectorTool.FillCacheInitTime = 0 # default is 1
    if ( hasattr(simFlags, 'SimulateNewSmallWheel') and simFlags.SimulateNewSmallWheel() ) or CommonGeometryFlags.Run()=="RUN3" :
        MuonDetectorTool.StationSelection  = 2
        MuonDetectorTool.SelectedStations  = [ "EIL1" ]
        MuonDetectorTool.SelectedStations  += [ "EIL2" ]
        MuonDetectorTool.SelectedStations  += [ "EIL6" ]
        MuonDetectorTool.SelectedStations  += [ "EIL7" ]
        MuonDetectorTool.SelectedStations  += [ "EIS*" ]
        MuonDetectorTool.SelectedStations  += [ "EIL10" ]
        MuonDetectorTool.SelectedStations  += [ "EIL11" ]
        MuonDetectorTool.SelectedStations  += [ "EIL12" ]
        MuonDetectorTool.SelectedStations  += [ "EIL17" ]
        MuonDetectorTool.SelectedStations  += [ "CSS*" ]
        MuonDetectorTool.SelectedStations  += [ "CSL*" ]
        MuonDetectorTool.SelectedStations  += [ "T4E*" ]
        MuonDetectorTool.SelectedStations  += [ "T4F*" ]

    ## Additional material in the muon system
    from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
    AGDD2Geo = AGDDtoGeoSvc()
    from AthenaCommon import CfgGetter
    if not "MuonAGDDTool/MuonSpectrometer" in AGDD2Geo.Builders:
        ToolSvc += CfgGetter.getPublicTool("MuonSpectrometer", checkType=True)
        AGDD2Geo.Builders += ["MuonAGDDTool/MuonSpectrometer"]
    if ( hasattr(simFlags, 'SimulateNewSmallWheel') and simFlags.SimulateNewSmallWheel() ) or CommonGeometryFlags.Run()=="RUN3" :
        if not "NSWAGDDTool/NewSmallWheel" in AGDD2Geo.Builders:
            ToolSvc += CfgGetter.getPublicTool("NewSmallWheel", checkType=True)
            AGDD2Geo.Builders += ["NSWAGDDTool/NewSmallWheel"]
    theApp.CreateSvc += ["AGDDtoGeoSvc"]
    ServiceMgr += AGDD2Geo

## Add configured GeoModelSvc to service manager
ServiceMgr += gms

## AtlasSimSkeleton._do_metadata
from G4AtlasApps.SimFlags import simFlags
if not simFlags.ISFRun:
    from G4AtlasApps.G4Atlas_Metadata import createSimulationParametersMetadata
    createSimulationParametersMetadata()
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if not athenaCommonFlags.PoolHitsOutput.statusOn:
        print 'AtlasSimSkeleton._do_metadata :: no output HITS file, so no metadata writing required.'
    else:
        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
        stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]

if not simFlags.ISFRun:
    def hits_persistency():
        """ HITS POOL file persistency
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.DetFlags import DetFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

        ## Not yet understood, but need to treat StreamHITS as alg in Hive.
        ## Seems to also work fine outside of Hive, but to be extra safe I'm
        ## only changing it in Hive.
        from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
        if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
            as_alg = True
        else:
            as_alg = False
        ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
        stream1 = AthenaPoolOutputStream("StreamHITS", athenaCommonFlags.PoolHitsOutput(), asAlg=as_alg)

        ## Write geometry tag info - move to main method
        #import EventInfoMgt.EventInfoMgtInit

        ## EventInfo & TruthEvent always written by default
        stream1.ForceRead=True
        stream1.ItemList = ["EventInfo#*",
                            "McEventCollection#TruthEvent",
                            "JetCollection#*"]

        ## If we are running quasi-stable particle simulation, include the original event record
        if hasattr(simFlags,'IncludeParentsInG4Event') and simFlags.IncludeParentsInG4Event.statusOn and simFlags.IncludeParentsInG4Event():
            stream1.ItemList += ["McEventCollection#GEN_EVENT"]

        stream1.ItemList += ["xAOD::JetContainer#*",
                             "xAOD::JetAuxContainer#*"]

        ## Make stream aware of aborted events
        stream1.AcceptAlgs = ["G4AtlasAlg"]

        ## Detectors

        ## Inner Detector
        if DetFlags.ID_on():
            stream1.ItemList += ["SiHitCollection#*",
                                 "TRTUncompressedHitCollection#*",
                                 "TrackRecordCollection#CaloEntryLayer"]
        ## Calo
        if DetFlags.Calo_on():
            stream1.ItemList += ["CaloCalibrationHitContainer#*",
                                 "LArHitContainer#*",
                                 "TileHitVector#*",
                                 #"SimpleScintillatorHitCollection#*",
                                 "TrackRecordCollection#MuonEntryLayer"]
        ## Muon
        if DetFlags.Muon_on():
            stream1.ItemList += ["RPCSimHitCollection#*",
                                 "TGCSimHitCollection#*",
                                 "CSCSimHitCollection#*",
                                 "MDTSimHitCollection#*",
                                 "TrackRecordCollection#MuonExitLayer"]
            if ( hasattr(simFlags, 'SimulateNewSmallWheel') and simFlags.SimulateNewSmallWheel() ) or CommonGeometryFlags.Run()=="RUN3" :
                stream1.ItemList += ["sTGCSimHitCollection#*"]
                stream1.ItemList += ["MMSimHitCollection#*"]
                stream1.ItemList += ["GenericMuonSimHitCollection#*"]

        ## Lucid
        if DetFlags.Lucid_on():
            stream1.ItemList += ["LUCID_SimHitCollection#*"]

        ## FwdRegion
        if DetFlags.FwdRegion_on():
            stream1.ItemList += ["SimulationHitCollection#*"]

        ## ZDC
        if DetFlags.ZDC_on():
            stream1.ItemList += ["ZDC_SimPixelHit_Collection#*",
                                 "ZDC_SimStripHit_Collection#*"]
        ## ALFA
        if DetFlags.ALFA_on():
            stream1.ItemList += ["ALFA_HitCollection#*",
                                 "ALFA_ODHitCollection#*"]

        ## AFP
        if DetFlags.AFP_on():
            stream1.ItemList += ["AFP_TDSimHitCollection#*",
                                 "AFP_SIDSimHitCollection#*"]

        ### Ancillary scintillators
        #stream1.ItemList += ["ScintillatorHitCollection#*"]

        ## TimingAlg
        stream1.ItemList +=["RecoTimingObj#EVNTtoHITS_timings"]

        ## Add cosmics and test beam configuration hit persistency if required cf. geom tag
        layout = simFlags.SimLayout.get_Value()
        if "tb" not in layout:
            from AthenaCommon.BeamFlags import jobproperties
            if jobproperties.Beam.beamType() == 'cosmics' or \
                    (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn) or \
                    (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
                stream1.ItemList += ["TrackRecordCollection#CosmicRecord", "TrackRecordCollection#CosmicPerigee"]
        else:
            ## CTB-specific
            if layout.startswith("ctb"):
                if simFlags.LArFarUpstreamMaterial.statusOn and simFlags.LArFarUpstreamMaterial.get_Value():
                    stream1.ItemList.append("TrackRecordCollection#LArFarUpstreamMaterialExitLayer")
            ## Persistency of test-beam layout
            if layout.startswith('ctb') or layout.startswith('tb_Tile2000_'):
                stream1.ItemList += ["TBElementContainer#*"]


    def evgen_persistency():
        """ EVGEN POOL file persistency
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
        ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
        if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
            stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.WriteTR.get_Value())
            stream2.ItemList += ["IOVMetaDataContainer#*",
                                 "EventInfo#*"]
            if simFlags.CavernBG.statusOn and 'Write' in simFlags.CavernBG.get_Value():
                stream2.ItemList += ["TrackRecordCollection#NeutronBG"]
            else:
                stream2.ItemList += ["TrackRecordCollection#CosmicRecord"]
            stream2.AcceptAlgs = ["G4AtlasAlg"]
        if hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
            stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.StoppedParticleFile.get_Value())
            stream2.ItemList += ["IOVMetaDataContainer#*",
                                 "EventInfo#*"]
            stream2.ItemList += ["TrackRecordCollection#StoppingPositions"]
            stream2.AcceptAlgs = ["G4AtlasAlg"]


    def will_write_output_files():
        """ Check if any POOL files will be written by this job
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.PoolHitsOutput.statusOn:
            return True
        elif ("tb" not in simFlags.SimLayout.get_Value()):
            if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
                return True
            elif hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
                return True
        return False


    def do_run_number_modifications():
        """ Set the run number in the simulation.  In order of priority, use:
             - The RunNumber flag
             - The input file run number """
        # FIXME This method is called from both _do_persistency and _do_All for AtlasG4 jobs!
        from G4AtlasApps.G4Atlas_Metadata import configureRunNumberOverrides
        configureRunNumberOverrides()


    ## SimSkeleton._do_readevgen
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    ## ReadTR is only present in simFlags for ATLAS geometries with cosmics switched on
    if (not simFlags.ISFRun) and hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn:
        include("CosmicGenerator/SetCosmicGenerator.py")

    if athenaCommonFlags.PoolEvgenInput.statusOn:
        ## Tell the event selector about the evgen input files and event skipping
        if not hasattr(svcMgr, 'EventSelector'):
            import AthenaPoolCnvSvc.ReadAthenaPool
        svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
        if athenaCommonFlags.SkipEvents.statusOn:
            svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
        from G4AtlasApps.G4Atlas_Metadata import inputFileValidityCheck
        inputFileValidityCheck()
    else:
        ## No input file so assume that we are running a Generator in the same job
        if not hasattr(svcMgr, 'EventSelector'):
            import AthenaCommon.AtlasUnixGeneratorJob
        # TODO: Check that there is at least one algorithm already in the AlgSequence?
        ## Warn if attempting to skip events in a generator job
        if athenaCommonFlags.SkipEvents.statusOn and athenaCommonFlags.SkipEvents()!=0:
            msg = "SimSkeleton._do_readevgen :: athenaCommonFlags.SkipEvents set in a job without an active "
            msg += "athenaCommonFlags.PoolEvgenInput flag: ignoring event skip request"
            print msg

    ## SimSkeleton._do_persistency
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if will_write_output_files():
        ## Write hits in POOL
        ## The following used to be in G4AtlasApps/HitAthenaPoolWriteOptions
        from AthenaCommon.DetFlags import DetFlags
        from AthenaCommon.Configurable import Configurable
        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

        ## Default setting for one output stream
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
        # Recommendations from Peter vG 16.08.25
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolHitsOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'" ]

        ## Write geometry tag info
        import EventInfoMgt.EventInfoMgtInit

        ## Patch metadata if required
        from G4AtlasApps.G4Atlas_Metadata import patch_mc_channel_numberMetadata
        patch_mc_channel_numberMetadata()

        ## Instantiate StreamHITS
        if athenaCommonFlags.PoolHitsOutput.statusOn:
            hits_persistency()

        ## StreamEVGEN: needed for cosmic simulations and cavern BG
        ## Separate stream of track record (TR) info -- it does not apply to the CTB simulations.
        # TODO: Can this be merged into the cosmics sec above, or do the AthenaPool includes *need* to be in-between?
        if "tb" not in simFlags.SimLayout.get_Value():
            evgen_persistency()

    # Check on run numbers and update them if necessary
    do_run_number_modifications()


## _PyG4AtlasComp.initialize
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if athenaCommonFlags.EvtMax.statusOn:# and theApp.EvtMax == -1:
    theApp.EvtMax = athenaCommonFlags.EvtMax()
