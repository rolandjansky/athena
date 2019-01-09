## _PyG4AtlasComp.__init__
## If the random number service hasn't been set up already, do it now.
from G4AtlasApps.SimFlags import simFlags
simFlags.RandomSeedList.useDefaultSeeds()

## Tile2000_2003._do_jobproperties
## Import extra flags if it hasn't already been done
from G4AtlasApps.SimFlags import simFlags
if not simFlags.extra_flags:
    simFlags.load_tbtile_flags()

# - switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulateLVL1.all_setOff()
DetFlags.digitize.all_setOff()
DetFlags.overlay.all_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.writeRDOPool.all_setOff()

# not needed detectors
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.FCal_setOff()
DetFlags.HEC_setOff()
DetFlags.em_setOff()
#
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = "geant4"
globalflags.DetGeo = "ctbh8"
# mgallas this is a hack! needed for GeoModel and IOVDbSvc
# TODO: Wow! Is this really still needed?
import __main__
__main__.DetDescrVersion = 'CTB'
__main__.globalflags = globalflags
globalflags.DetDescrVersion = 'CTB'

## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
from AthenaCommon.AppMgr import ServiceMgr
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
ServiceMgr += IOVDbSvc()
if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
    msg = "Tile2000_2003._do_jobproperties :: conditions tag has not been set and is no longer guessed by SimAtlasKernel. "
    msg += "You must set the globaltags.ConditionsTag flag in your job options."
    print msg
    raise SystemExit("Tile2000_2003._do_jobproperties :: Global ConditionsTag not set")
if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
    ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

if not simFlags.ISFRun:
    from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
    checkForSpecialConfigurationMetadata()

## Print flags
from AthenaCommon.JobProperties import jobproperties
#if AtlasG4Eng.G4Eng.log.getEffectiveLevel()<40:
#    AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing detector flags DetFlags')
#    DetFlags.Print()
#    AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing simulation flags simFlags')
#    jobproperties.print_JobProperties('tree&value')
jobproperties.lock_JobProperties()

## Tile2000_2003._do_external
from AthenaCommon.AppMgr import ServiceMgr
from Geo2G4.Geo2G4Conf import Geo2G4Svc
Geo2G4Svc=Geo2G4Svc()
theApp.CreateSvc += [ "Geo2G4Svc" ]
ServiceMgr +=Geo2G4Svc
Geo2G4Svc.GetTopTransform = False

from AtlasGeoModel import SetGeometryVersion
from AthenaCommon.Configurable import Configurable
if Configurable.allConfigurables.get('GeoModelSvc'):
    GeoModelSvc=Configurable.allConfigurables.get('GeoModelSvc')
else:
    GeoModelSvc=theApp.service('GeoModelSvc')
GeoModelSvc.AlignCallbacks =False

from G4AtlasApps.SimFlags import simFlags
if (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_2B2EB'):
    # 2 Barrels + 2 Extended Barrels
    GeoModelSvc.TileVersionOverride='TileTB-2B2EB-00'
elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_2B1EB'):
    # 2 Barrels + 1 Extended Barrel
    GeoModelSvc.TileVersionOverride='TileTB-2B1EB-00'
elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_3B'):
    # 3 Barrels
    GeoModelSvc.TileVersionOverride='TileTB-3B-00'
elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_5B'):
    # 5 Barrels
    GeoModelSvc.TileVersionOverride='TileTB-5B-00'

from AtlasGeoModel import GeoModelInit
if(DetFlags.Calo_on()):
    # Common for the Calo
    include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
    include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
##--- Persistency of test-beam layout ----------------------------------
if jobproperties.AthenaCommonFlags.PoolHitsOutput.statusOn :
    include( "TBDetDescrAthenaPool/TBDetDescrAthenaPool_joboptions.py" )
    from TBDetDescrAlg.TBDetDescrAlgConf import TBDetDescrLoader
    from AthenaCommon.AlgSequence import AlgSequence
    job=AlgSequence()
    TBDetDescrLoader=TBDetDescrLoader()
    job+=TBDetDescrLoader
    TBDetDescrLoader.TBElementContainer = "TBElementCnt"
    TBDetDescrLoader.TBDetDescrManager  = "TBDetDescrMgr"
    include( "TBDetDescrCnv/TBDetDescrCnv_jobOptions.py" )
    # do not read anything from StoreGate
    TBDetDescrLoader.ReadAction = 0
    # and write TBDetDescrContainer to StoreGate every event
    TBDetDescrLoader.WriteAction = 2
    TBDetDescrLoader.OutputLevel = 5

## Explicitly create DetectorGeometrySvc - temporary fix
from AthenaCommon.CfgGetter import getService, getPublicTool
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += getService('DetectorGeometrySvc')
ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')

## TBSimSkeleton._do_metadata
from G4AtlasApps.G4Atlas_Metadata import createTBSimulationParametersMetadata
createTBSimulationParametersMetadata()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if (athenaCommonFlags.PoolHitsOutput.statusOn) :
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
            from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
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
