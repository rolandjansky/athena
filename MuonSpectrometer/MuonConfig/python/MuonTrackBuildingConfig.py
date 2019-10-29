# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MuonRecToolsConfig import MCTBFitterCfg, MuonSTEP_PropagatorCfg, MuonTrackCleanerCfg, MuonSegmentMomentumFromFieldCfg, MuonSeededSegmentFinderCfg
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFittingToolCfg 

def MooTrackFitterCfg(flags, name = 'MooTrackFitter', **kwargs):
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L179
    # (Tons of explicit configuration missing from there though)
    from MuonTrackSteeringTools.MuonTrackSteeringToolsConf import Muon__MooTrackFitter
    from MuonSegmentMomentum.MuonSegmentMomentumConf import MuonSegmentMomentum
    
    result = MCTBFitterCfg(flags)
    mctb_fitter = result.getPrimary()
    result.addPublicTool(mctb_fitter)
    kwargs.setdefault("Fitter",          mctb_fitter)
    
    acc = MuonSTEP_PropagatorCfg(flags)
    muon_prop = acc.getPrimary()
    result.addPublicTool(muon_prop)
    result.merge(acc)
    
    kwargs.setdefault("Propagator",      muon_prop)
    # kwargs.setdefault("SLFit" ,          ) # Was "not jobproperties.BField.allToroidOn()" but do not have access to Field here.
    kwargs.setdefault("ReducedChi2Cut",  flags.Muon.Chi2NDofCut)
    
    momentum_estimator=""
    if flags.Beam.Type == 'cosmics':
        momentum_estimator = MuonSegmentMomentum(DoCosmics = True)
    else:
        acc = MuonSegmentMomentumFromFieldCfg(flags)
        momentum_estimator=acc.getPrimary()
        result.merge(acc)
        
    result.addPublicTool(momentum_estimator)
    kwargs.setdefault("SegmentMomentum", momentum_estimator )
    
    kwargs.setdefault("CleanPhiHits",              True)
    kwargs.setdefault("UsePreciseHits",            True)
    kwargs.setdefault("UsePrefit",                 False)
    kwargs.setdefault("SeedAtStartOfTrack",        False)
    
    acc = MuonTrackCleanerCfg(flags)
    track_cleaner = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(track_cleaner)
    kwargs.setdefault("TrackCleaner",  track_cleaner)
    
    fitter = Muon__MooTrackFitter(name, **kwargs)
    result.setPrivateTools(fitter)
    return result

def MooTrackBuilderCfg(flags, name="MooTrackBuilderTemplate", **kwargs):
    from MuonTrackSteeringTools.MuonTrackSteeringToolsConf import Muon__MooTrackBuilder
    from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
    
    # Based on this: https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L221
    # ignoring all the name_prefix stuff for the moment, since I'm not sure it's necessary any more.
    result=MooTrackFitterCfg(flags)
    moo_track_fitter = result.getPrimary()
    result.addPublicTool(moo_track_fitter)
    kwargs.setdefault("Fitter", moo_track_fitter)
    
    # Now setup SL fitter (MooSLTrackFitter as defined here:
    # https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecExampleConfigDb.py#L203)
    acc = MCTBFitterCfg(flags, name='MCTBSLFitter', StraightLine=True)
    mctbslfitter = acc.getPrimary()
    result.addPublicTool(mctbslfitter)
    
    result.merge(acc)
    
    # Just take the default configuration, as per https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecExampleConfigDb.py#L56
    # FIXME - this should be updated once there is a proper CA implementation for the STEP_Propagator
    prop = Trk__STEP_Propagator(name = 'MuonStraightLinePropagator')
    result.addPublicTool(prop)
    
    # TODO - check why Fitter and FitterPreFit are identical
    acc = MooTrackFitterCfg( flags, name="MooSLTrackFitter", Fitter = mctbslfitter, FitterPreFit=mctbslfitter, Propagator=prop, ReducedChi2Cut=10.0,  SLFit=True)
    moo_sl_track_fitter = acc.getPrimary()
    result.addPublicTool(moo_sl_track_fitter)
    result.merge(acc)
    
    kwargs.setdefault("SLFitter", moo_sl_track_fitter)
    kwargs.setdefault("RecalibrateMDTHitsOnTrack", ( (not flags.Muon.doSegmentT0Fit) and flags.Beam.Type == 'collisions') )
    
    
    acc = MuonSeededSegmentFinderCfg(flags)
    muon_seeded_segment_finder = acc.getPrimary()
    result.addPublicTool(muon_seeded_segment_finder)
    result.merge(acc)
    kwargs.setdefault("SeededSegmentFinder", muon_seeded_segment_finder)
    
    builder = Muon__MooTrackBuilder(name, **kwargs)
    result.setPrivateTools(builder)
    return result

# Taken from https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L268
# Not bothering with MuonSegmentMatchingToolTight - just pass in TightSegmentMatching=True
def MuonSegmentMatchingToolCfg(flags, name="MuonSegmentMatchingTool", **kwargs):
    from MuonSegmentMatchingTools.MuonSegmentMatchingToolsConf import Muon__MuonSegmentMatchingTool
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    
    kwargs.setdefault( "doThetaMatching", flags.Muon.useSegmentMatching)
    kwargs.setdefault( "doPhiMatching", False )
    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("OverlapMatchAveragePhiHitPullCut",  200.)
            
    # There are two tools which this depends on which aren't properties and which weren't defined in old configuration
    # Ignore for now, but FIXME one day
    # m_overlapResolvingTool("Muon::MuonSegmentInOverlapResolvingTool/MuonSegmentInOverlapResolvingTool"),
    # m_pairMatchingTool("Muon::MuonSegmentPairMatchingTool/MuonSegmentPairMatchingTool"),
    # Also, residual pull calculator not yet configured. #FIXME
    
    result  = MagneticFieldSvcCfg(flags) 
    magfieldsvc = result.getPrimary()
    
    kwargs.setdefault( "MagFieldSvc", magfieldsvc )
    
    matching = Muon__MuonSegmentMatchingTool(name, **kwargs)
    result.setPrivateTools(matching)
    return result
    
def MooCandidateMatchingToolCfg(flags, name="MooCandidateMatchingTool", doSegmentPhiMatching=True, **kwargs):
    from MuonTrackSteeringTools.MuonTrackSteeringToolsConf import Muon__MooCandidateMatchingTool
    result = ComponentAccumulator()
    if doSegmentPhiMatching is not None:
        acc = MuonSegmentMatchingToolCfg(flags, doPhiMatching = doSegmentPhiMatching)
        muon_seg_matching = acc.getPrimary()
        result.addPublicTool(muon_seg_matching)
        result.merge(acc)
        kwargs.setdefault("SegmentMatchingTool",      muon_seg_matching)
    
        acc = MuonSegmentMatchingToolCfg(flags, name = "MuonSegmentMatchingToolTight", TightSegmentMatching=True, doPhiMatching = doSegmentPhiMatching)
        muon_seg_matching_tight = acc.getPrimary()
        result.addPublicTool(muon_seg_matching_tight)
        result.merge(acc)
        kwargs.setdefault("SegmentMatchingToolTight", muon_seg_matching_tight)
        
    kwargs.setdefault("DoTrackSegmentMatching", flags.Muon.useTrackSegmentMatching)
    kwargs.setdefault("RequireSameSide", flags.Beam.Type != 'collisions')
    if flags.Muon.useAlignmentCorrections:
        kwargs.setdefault("AlignmentErrorPosX",   5.0)
        kwargs.setdefault("AlignmentErrorPosY",   0.2)
        kwargs.setdefault("AlignmentErrorAngleX", 0.002)
        kwargs.setdefault("AlignmentErrorAngleY", 0.001)

    else: # no alignment corrections
        kwargs.setdefault("AlignmentErrorPosX",   10.0)
        kwargs.setdefault("AlignmentErrorPosY",   5.0)
        kwargs.setdefault("AlignmentErrorAngleX", 0.004)
        kwargs.setdefault("AlignmentErrorAngleY", 0.002)

    moo_cand_matching_tool = Muon__MooCandidateMatchingTool(name,**kwargs)
    result.setPrivateTools(moo_cand_matching_tool)
    return result

def MuonSegmentRegionRecoveryToolCfg(flags, name="MuonSegmentRegionRecoveryTool", **kwargs):
    from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonSegmentRegionRecoveryTool
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L426
    result = MCTBFitterCfg(flags)
    mctbfitter = result.getPrimary()
    result.addPublicTool(mctbfitter)
    kwargs.setdefault("Fitter", mctbfitter)
    
    acc = MooCandidateMatchingToolCfg(flags)
    track_segment_matching_tool=acc.getPrimary()
    result.addPublicTool(track_segment_matching_tool)
    result.merge(acc)
    kwargs.setdefault("TrackSegmentMatchingTool", track_segment_matching_tool)
    
    segment_region_recovery_tool = Muon__MuonSegmentRegionRecoveryTool(name, **kwargs)
    result.setPrivateTools(segment_region_recovery_tool)
    return result
    
def MuPatCandidateToolCfg(flags, name="MuPatCandidateTool", **kwargs):
    from MuonTrackSteeringTools.MuonTrackSteeringToolsConf import Muon__MuPatCandidateTool
    # https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuPatTools.py#L32

    from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg,MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_dcot_creator = result.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)
    
    acc = CscClusterOnTrackCreatorCfg(flags)
    csc_cluster_creator = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CscRotCreator", csc_cluster_creator)
    
    kwargs.setdefault("SegmentExtender", "") #FIXME If this is always empty, can it be removed?
    
    from AthenaCommon.Constants import VERBOSE
    mu_pat_cand_tool = Muon__MuPatCandidateTool(name, OutputLevel=VERBOSE, **kwargs)
    result.setPrivateTools(mu_pat_cand_tool)
    return result
    
def MuonChamberHoleRecoveryToolCfg(flags, name="MuonChamberHoleRecoveryTool", **kwargs):
    from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonChamberHoleRecoveryTool
    result=ComponentAccumulator()
    kwargs.setdefault("AddMeasurements",  not flags.Muon.doSegmentT0Fit )
    if flags.Detector.GeometryCSC:
        from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg
        extrakwargs={}
        if flags.Muon.enableErrorTuning or not flags.Input.isMC:
            extrakwargs["ErrorScalerBeta"] = 0.200
        acc = CscClusterOnTrackCreatorCfg(flags, **extrakwargs)
        csc_cluster_creator = acc.getPrimary()
        result.merge(acc)
        kwargs.setdefault("CscRotCreator", csc_cluster_creator)
    else:
        kwargs["CscRotCreator"] = None
        kwargs.setdefault("CscPrepDataContainer","")
    
    if not flags.Detector.GeometrysTGC:
        kwargs.setdefault("sTgcPrepDataContainer","")
    
    if not flags.Detector.GeometryMM:
        kwargs.setdefault("MMPrepDataContainer","")
    
    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC and not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')    
    
    hole_rec_tool = Muon__MuonChamberHoleRecoveryTool(name, **kwargs)
    result.setPrivateTools(hole_rec_tool)
    return result
    

def MuonTrackSteeringCfg(flags, name="MuonTrackSteering", **kwargs):
    from MuonTrackSteeringTools.MuonTrackSteeringToolsConf import Muon__MuonTrackSteering
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py#L35
    result=ComponentAccumulator()
    
    # This was originally defined here: https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandaloneFlags.py#L137
    MoorelikeStrategy=[
         "BarrelCombined[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:BML,BMS;BOL,BOS;BIL,BIS",
         "EndcapCombined[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:EML,EMS;EOL,EOS;EIL,EIS;CSL,CSS;EEL,EES;BEE",
         "BarrelEndcap[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks,BarrelEndcapFilter]:EML,EMS;EEL,EES;BML,BMS;EIL,EIS;BIL,BIS;BOL,BOS"]
    
    kwargs.setdefault("StrategyList", MoorelikeStrategy) 
    
    # Set properties which were set in MuonStandalone.py, and which aren't default (fix this later, because we don't want to break existing configuration)
    # extraFlags are ignored (don't believe they're necessary)
    
    result = MCTBFitterCfg(flags, name='MCTBSLFitter', StraightLine=True)
    mctbslfitter = result.getPrimary()
    result.addPublicTool(mctbslfitter)
    
    acc = MuonSegmentRegionRecoveryToolCfg(flags, name='MuonEORecoveryTool', OnlyEO = True, Fitter=mctbslfitter, UseFitterOutlierLogic=False)
    muon_eo_recovery_tool = acc.getPrimary()
    result.addPublicTool(muon_eo_recovery_tool)
    result.merge(acc) 
    
    kwargs.setdefault("HoleRecoveryTool",       muon_eo_recovery_tool)
    kwargs.setdefault("SegSeedQCut", 2)
    kwargs.setdefault("Seg2ndQCut", 1)
    
    kwargs.setdefault("OutputSingleStationTracks", True) # default is False

    if "TrackBuilderTool" not in kwargs:
        #Old config had: kwargs["TrackBuilderTool"] = getPublicToolClone("MooMuonTrackBuilder", "MooTrackBuilderTemplate",
        #                                                extraFlags=extraFlags)
        # For some strange reason, MooTrackBuilderTemplate was just a MooTrackBuilder i.e. they had:
        # addTool( "MuonRecExample.MooreTools.MooTrackBuilder", "MooTrackBuilderTemplate")
        # I have simplified this. Also, in the old configuration there was the possiblity to set optimiseMomentumResolutionUsingChi2 to true in extraflags (which were passed to TrackBuilderTool but not TrackRefinementTool) - however I don't think this was used, and have removed it.
        # However some other things were set in extraFlags here: https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py#L39
        # So I might well have made a mistake. TODO check
        acc = MooTrackBuilderCfg(flags)
        builder = acc.getPrimary()
        kwargs["TrackBuilderTool"] = builder
        result.merge(acc)
        
        if "TrackRefinementTool" not in kwargs:
            kwargs["TrackRefinementTool"] = builder
            # FIXME. Need to see if TrackRefinementTool is actually used & possibly remove.
    
    # --- Now let's setup tools which were ignored in the old configuration. Just want to get this working for the moment. Will clean it up later.

    acc=MuPatCandidateToolCfg(flags)
    cand_tool = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(cand_tool)
    kwargs.setdefault("MuPatCandidateTool",       cand_tool) 
    
    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg
    acc  = MuonAmbiProcessorCfg(flags, name='MuonAmbiProcessor')
    ambi = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(ambi)
    kwargs.setdefault("AmbiguityTool",       ambi) 
    
    kwargs.setdefault("MooBuilderTool",       builder) 
    kwargs.setdefault("TrackRefinementTool",       builder) 
    
    acc = MooCandidateMatchingToolCfg(flags)
    cand_matching_tool = acc.getPrimary()
    kwargs["CandidateMatchingTool"] = cand_matching_tool
    result.merge(acc)
    
    acc = MuonSegmentFittingToolCfg(flags)
    seg_fitting_tool =  acc.getPrimary()
    result.merge(acc)
    
    kwargs.setdefault("MuonSegmentFittingTool",       seg_fitting_tool) 
    kwargs.setdefault("MuonTrackSelector",       MuonTrackSelector(flags)) 

    acc = MuonChamberHoleRecoveryToolCfg(flags)
    hole_recovery_tool =  acc.getPrimary()
    result.addPublicTool(hole_recovery_tool)
    result.merge(acc)
    
    kwargs.setdefault("HoleRecoveryTool",       hole_recovery_tool) 
    
    track_maker_steering = Muon__MuonTrackSteering(name,**kwargs)
    result.setPrivateTools(track_maker_steering)
    return result

def MuonTrackSelector(flags, name = "MuonTrackSelectorTool", **kwargs):
    from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonTrackSelectorTool
    # In MooreTools this is:
    # if beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' or globalflags.DataSource() == 'data' :
    # Hopefully this is good enough
    if flags.Beam.Type == 'cosmics' or not flags.Input.isMC:
        kwargs.setdefault("UseRPCHoles", False) 
        kwargs.setdefault("UseTGCHoles", False) 
        kwargs.setdefault("MaxMdtHolesOnTwoStationTrack", 10) 
        kwargs.setdefault("MaxMdtHolesOnTrack", 30) 
        kwargs.setdefault("HolesToHitsRatioCutPerStation", 2) 
        kwargs.setdefault("CountMDTOutlierAsHoles", False) 
    else:
        kwargs.setdefault("MaxMdtHolesOnTwoStationTrack", 4) 
        kwargs.setdefault("MaxMdtHolesOnTrack", 5) 
        kwargs.setdefault("CountMDTOutlierAsHoles", True)
    
    return Muon__MuonTrackSelectorTool(name, **kwargs)

def MuonTrackBuildingCfg(flags):
    from MuonSegmentTrackMaker.MuonSegmentTrackMakerConf import MuPatTrackBuilder
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py#L162
    result=ComponentAccumulator()
    
    #Arguably this should be somewhere deeper - not quite sure where yet though. FIXME
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg 
    result.merge( MuonGeoModelCfg(flags) )
    
    from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    result.addService( muon_edm_helper_svc )
    
    acc = MuonTrackSteeringCfg(flags)
    track_steering = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(track_steering)
    
    track_builder = MuPatTrackBuilder("MuPatTrackBuilder", TrackSteering = track_steering, MuonSegmentCollection="MuonSegments", SpectrometerTrackOutputLocation="MuonSpectrometerTracks" )

    result.addEventAlgo( track_builder, primary=True )
    return result
    

if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonConfig.MuonTrackBuildingConfig --run --threads=
    
    from argparse import ArgumentParser    
    parser = ArgumentParser()
    parser.add_argument("-t", "--threads", dest="threads", type=int,
                        help="number of threads", default=1)
                        
    parser.add_argument("-o", "--output", dest="output", default='newESD.pool.root',
                        help="write ESD to FILE", metavar="FILE")
                        
    parser.add_argument("--run", help="Run directly from the python. If false, just stop once the pickle is written.",
                        action="store_true")
                        
    args = parser.parse_args()
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.Concurrency.NumThreads=args.threads
    ConfigFlags.Concurrency.NumConcurrentEvents=args.threads # Might change this later, but good enough for the moment.

    ConfigFlags.Detector.GeometryMDT   = True 
    ConfigFlags.Detector.GeometryTGC   = True
    ConfigFlags.Detector.GeometryCSC   = True     
    ConfigFlags.Detector.GeometryRPC   = True 
        
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.Output.ESDFileName=args.output

    # from AthenaCommon.Constants import DEBUG
    # log.setLevel(DEBUG)
    # log.info('About to set up Muon Track Building.')
    
    ConfigFlags.Input.isMC = True
    ConfigFlags.lock()
    ConfigFlags.dump()

    # When running from a pickled file, athena inserts some services automatically. So only use this if running now.
    if args.run:
        from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
        cfg = MainServicesThreadedCfg(ConfigFlags)
    else:
        cfg=ComponentAccumulator()
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    log.debug('About to set up Muon Track Building.')    
    acc = MuonTrackBuildingCfg(ConfigFlags)
    cfg.merge(acc)
    
    if args.threads>1:
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        from GaudiHive.GaudiHiveConf import AlgResourcePool
        cfg.addService(AlgResourcePool( OverrideUnClonable=True ) )
        track_builder = acc.getPrimary()
        track_builder.Cardinality=args.threads
            
    # This is a temporary fix - it should go someplace central as it replaces the functionality of addInputRename from here:
    # https://gitlab.cern.ch/atlas/athena/blob/master/Control/SGComps/python/AddressRemappingSvc.py
    from SGComps.SGCompsConf import AddressRemappingSvc, ProxyProviderSvc
    pps = ProxyProviderSvc()
    ars=AddressRemappingSvc()
    pps.ProviderNames += [ 'AddressRemappingSvc' ]
    ars.TypeKeyRenameMaps += [ '%s#%s->%s' % ("TrackCollection", "MuonSpectrometerTracks", "MuonSpectrometerTracks_old") ]
    
    cfg.addService(pps)
    cfg.addService(ars)
    
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    itemsToRecord = ["TrackCollection#MuonSpectrometerTracks"] 

    cfg.merge( OutputStreamCfg( ConfigFlags, 'ESD', ItemList=itemsToRecord) )
    
    outstream = cfg.getEventAlgo("OutputStreamESD")
    # outstream.OutputLevel=DEBUG
    outstream.ForceRead = True
    
    # Show slots & events
    # msgService = cfg.getService('MessageSvc')
    # msgService.Format = "S:%s E:%e % F%48W%S%7W%R%T  %0W%M"
    # msgService.OutputLevel=DEBUG
    
    # Fix for ATLASRECTS-5151
    from  TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
    cnvTool = Trk__EventCnvSuperTool(name = 'EventCnvSuperTool')
    cnvTool.MuonCnvTool.FixTGCs = True
    cfg.addPublicTool(cnvTool)
    
    # from MuonEventCnvTools.MuonEventCnvToolsConf import Muon__MuonEventCnvTool
    # cnvTool = Muon__MuonEventCnvTool(name='MuonEventCnvTool')
    # cnvTool.FixTGCs = True
    # cfg.addPublicTool(cnvTool)
    
    cfg.printConfig(withDetails = True, summariseProps = True)
              
    f=open("MuonTrackBuilding.pkl","w")
    cfg.store(f)
    f.close()

    if args.run:
        cfg.run(20)
