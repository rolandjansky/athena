# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MuonConfig.MuonRecToolsConfig import MCTBFitterCfg, MuonSTEP_PropagatorCfg, MuonTrackCleanerCfg, MuonSegmentMomentumFromFieldCfg, MuonSeededSegmentFinderCfg, MuonEDMPrinterTool
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFittingToolCfg 

def MooTrackFitterCfg(flags, name = 'MooTrackFitter', **kwargs):
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L179
    # (Tons of explicit configuration missing from there though)
    Muon__MooTrackFitter=CompFactory.Muon.MooTrackFitter
    MuonSegmentMomentum=CompFactory.MuonSegmentMomentum
    from MuonConfig.MuonRecToolsConfig import MuonPhiHitSelector, MuonTrackToSegmentToolCfg, MuonTrackSummaryHelperToolCfg, MuPatHitToolCfg
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg
    
    result = MCTBFitterCfg(flags)
    mctb_fitter = result.getPrimary()
    result.addPublicTool(mctb_fitter)
    kwargs.setdefault("Fitter",          mctb_fitter)
        
    acc = MuPatHitToolCfg(flags)
    mu_pat_hit_tool = acc.getPrimary()
    result.addPublicTool(mu_pat_hit_tool)
    result.merge(acc)
    kwargs.setdefault("HitTool",          mu_pat_hit_tool)
    
    acc = MuonSTEP_PropagatorCfg(flags)
    muon_prop = acc.getPrimary()
    result.addPublicTool(muon_prop)
    result.merge(acc)
    
    kwargs.setdefault("Propagator",      muon_prop)
    # kwargs.setdefault("SLFit" ,          ) # Was "not jobproperties.BField.allToroidOn()" but do not have access to Field here.
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("SLFit", False)
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
    
    kwargs.setdefault("MuonPrinterTool", MuonEDMPrinterTool(flags) )

    acc = MuonTrackToSegmentToolCfg(flags)
    track_to_segment_tool =  acc.getPrimary()
    kwargs.setdefault("TrackToSegmentTool", track_to_segment_tool)    
    result.merge(acc)
    
    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_dcot_creator = acc.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)
    result.merge(acc)
    
    kwargs.setdefault("PhiHitSelector",  MuonPhiHitSelector(flags))
    
    
    acc = MuonTrackCleanerCfg(flags)
    track_cleaner = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(track_cleaner)
    kwargs.setdefault("TrackCleaner",  track_cleaner)
    # Leaving "SegmentInOverlapTool" as default, which is what happens in the current configuration.
    
    acc = MuonTrackSummaryHelperToolCfg(flags)
    track_summary_helper = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("TrackSummaryTool",  track_summary_helper)
    
    
    kwargs.setdefault("CleanPhiHits",              True)
    kwargs.setdefault("UsePreciseHits",            True)
    kwargs.setdefault("UsePrefit",                 False)
    kwargs.setdefault("SeedAtStartOfTrack",        False)
    
    fitter = Muon__MooTrackFitter(name, **kwargs)
    result.setPrivateTools(fitter)
    return result

def MooTrackBuilderCfg(flags, name="MooTrackBuilderTemplate", **kwargs):
    Muon__MooTrackBuilder=CompFactory.Muon.MooTrackBuilder
    Trk__STEP_Propagator=CompFactory.Trk.STEP_Propagator
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg
    from MuonConfig.MuonRecToolsConfig import MuonTrackToSegmentToolCfg, MuonTrackExtrapolationToolCfg
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    
    # Based on this: https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L221
    # ignoring all the name_prefix stuff for the moment, since I'm not sure it's necessary any more.
    result=MooTrackFitterCfg(flags)
    moo_track_fitter = result.getPrimary()
    kwargs.setdefault("Fitter", moo_track_fitter)
    
    # Now setup SL fitter (MooSLTrackFitter as defined here:
    # https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecExampleConfigDb.py#L203)
    acc = MCTBFitterCfg(flags, name='MCTBSLFitter', StraightLine=True)
    mctbslfitter = acc.getPrimary()
    result.merge(acc)
    
    # Just take the default configuration, as per https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecExampleConfigDb.py#L56
    # FIXME - this should be updated once there is a proper CA implementation for the STEP_Propagator
    prop = Trk__STEP_Propagator(name = 'MuonStraightLinePropagator')
    result.addPublicTool(prop)
    
    acc = MooTrackFitterCfg( flags, name="MooSLTrackFitter", Fitter = mctbslfitter, Propagator=prop, ReducedChi2Cut=10.0,  SLFit=True)
    moo_sl_track_fitter = acc.getPrimary()
    result.merge(acc)
    
    kwargs.setdefault("SLFitter", moo_sl_track_fitter)
    kwargs.setdefault("RecalibrateMDTHitsOnTrack", ( (not flags.Muon.doSegmentT0Fit) and flags.Beam.Type == 'collisions') )
    
    acc = MuonSeededSegmentFinderCfg(flags)
    muon_seeded_segment_finder = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("SeededSegmentFinder", muon_seeded_segment_finder)

    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_dcot_creator = acc.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)
    result.merge(acc)
    
    acc = TriggerChamberClusterOnTrackCreatorCfg(flags)
    muon_comp_cluster_creator =  acc.getPrimary()
    kwargs.setdefault("CompetingClustersCreator", muon_comp_cluster_creator)    
    result.merge(acc)
    
    acc = MuonSTEP_PropagatorCfg(flags)
    muon_prop = acc.getPrimary()
    kwargs.setdefault("Propagator", muon_prop)    
    result.merge(acc)
    
    acc = MuonChamberHoleRecoveryToolCfg(flags)
    hole_recovery_tool =  acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("ChamberHoleRecoveryTool", hole_recovery_tool) 

    acc  = MagneticFieldSvcCfg(flags) 
    result.merge(acc)
    
    acc = MuonTrackToSegmentToolCfg(flags)
    track_to_segment_tool =  acc.getPrimary()
    kwargs.setdefault("TrackToSegmentTool", track_to_segment_tool)    
    result.merge(acc)
    
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )

    kwargs.setdefault('Extrapolator', result.popToolsAndMerge( MuonTrackExtrapolationToolCfg(flags) ) )

    # FIXME - remove ErrorOptimisationTool from cxx?
    # declareProperty("ErrorOptimisationTool","" );Extrapolator

    acc=MuPatCandidateToolCfg(flags)
    cand_tool = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CandidateTool",       cand_tool)

    acc = MooCandidateMatchingToolCfg(flags)
    track_segment_matching_tool=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CandidateMatchingTool", track_segment_matching_tool)

    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
    acc = MuonTrackSummaryToolCfg(flags)
    track_summary = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("TrackSummaryTool",  track_summary)
   
    builder = Muon__MooTrackBuilder(name, **kwargs)
    result.setPrivateTools(builder)
    return result

# Taken from https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L268
# Not bothering with MuonSegmentMatchingToolTight - just pass in TightSegmentMatching=True
def MuonSegmentMatchingToolCfg(flags, name="MuonSegmentMatchingTool", **kwargs):
    Muon__MuonSegmentMatchingTool=CompFactory.Muon.MuonSegmentMatchingTool
    
    kwargs.setdefault( "doThetaMatching", flags.Muon.useSegmentMatching)
    kwargs.setdefault( "doPhiMatching", False )
    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("OverlapMatchAveragePhiHitPullCut",  200.)
        kwargs.setdefault( "ToroidOn", False ) # Was "not jobproperties.BField.allToroidOn()" but do not have access to Field here.

    # There are two tools which this depends on which aren't properties and which weren't defined in old configuration
    # Ignore for now, but FIXME one day
    # m_overlapResolvingTool("Muon::MuonSegmentInOverlapResolvingTool/MuonSegmentInOverlapResolvingTool"),
    # m_pairMatchingTool("Muon::MuonSegmentPairMatchingTool/MuonSegmentPairMatchingTool"),
    # Also, residual pull calculator not yet configured. #FIXME
    
    result = ComponentAccumulator()
    matching = Muon__MuonSegmentMatchingTool(name, **kwargs)
    result.setPrivateTools(matching)
    return result
    
def MooCandidateMatchingToolCfg(flags, name="MooCandidateMatchingTool", doSegmentPhiMatching=False, **kwargs):
    Muon__MooCandidateMatchingTool=CompFactory.Muon.MooCandidateMatchingTool
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    
    result = ComponentAccumulator()

    # Won't explicitly configure MuonEDMHelperSvc
    kwargs.setdefault("MuonPrinterTool", MuonEDMPrinterTool(flags) )
    kwargs.setdefault("Extrapolator", result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    acc = MuonSegmentMatchingToolCfg(flags, doPhiMatching = doSegmentPhiMatching)
    muon_seg_matching = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("SegmentMatchingTool",      muon_seg_matching)

    acc = MuonSegmentMatchingToolCfg(flags, name = "MuonSegmentMatchingToolTight", TightSegmentMatching=True, doPhiMatching = doSegmentPhiMatching)
    muon_seg_matching_tight = acc.getPrimary()
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

    acc=MuPatCandidateToolCfg(flags)
    cand_tool = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MuPatCandidateTool",       cand_tool) 
    
    moo_cand_matching_tool = Muon__MooCandidateMatchingTool(name,**kwargs)
    result.setPrivateTools(moo_cand_matching_tool)
    return result

def MuonSegmentRegionRecoveryToolCfg(flags, name="MuonSegmentRegionRecoveryTool", **kwargs):
    Muon__MuonSegmentRegionRecoveryTool=CompFactory.Muon.MuonSegmentRegionRecoveryTool
    from MuonConfig.MuonRecToolsConfig import MuonExtrapolatorCfg, MuonStationIntersectSvcCfg, MuonTrackSummaryToolCfg
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L426
    
    result = MuonSeededSegmentFinderCfg(flags)
    muon_seeded_segment_finder = result.getPrimary()
    kwargs.setdefault("SeededSegmentFinder", muon_seeded_segment_finder)
    
    acc = MooCandidateMatchingToolCfg(flags)
    track_segment_matching_tool=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("TrackSegmentMatchingTool", track_segment_matching_tool)
    
    acc = MuonChamberHoleRecoveryToolCfg(flags)
    hole_recovery_tool =  acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("ChamberHoleRecoveryTool", hole_recovery_tool)
    
    acc = MuonExtrapolatorCfg(flags)
    extrap = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("Extrapolator", extrap)
    
    acc = MCTBFitterCfg(flags, name='MCTBSLFitter', StraightLine=True)
    mctbslfitter = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("Fitter", mctbslfitter)
    
    acc = MuonStationIntersectSvcCfg(flags)
    msis = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MuonStationIntersectSvc",msis)

    # Not bothering to handle IDHelper or EDMHelper or HitSummaryTool. Default is okay.
    
    from RegionSelector.RegSelToolConfig import regSelTool_MDT_Cfg, regSelTool_RPC_Cfg, regSelTool_TGC_Cfg
    acc = regSelTool_MDT_Cfg(flags)
    kwargs.setdefault("MDTRegionSelector", acc.popPrivateTools())
    result.merge(acc)

    acc = regSelTool_TGC_Cfg(flags)
    kwargs.setdefault("TGCRegionSelector", acc.popPrivateTools())
    result.merge(acc)

    acc = regSelTool_RPC_Cfg(flags)
    kwargs.setdefault("RPCRegionSelector", acc.popPrivateTools())
    result.merge(acc)

    if flags.Detector.GeometryCSC:
        from RegionSelector.RegSelToolConfig import regSelTool_CSC_Cfg
        acc = regSelTool_CSC_Cfg(flags)
        kwargs.setdefault("CSCRegionSelector", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("CSCRegionSelector", "")

    if flags.Detector.GeometrysTGC:
        from RegionSelector.RegSelToolConfig import regSelTool_STGC_Cfg
        acc = regSelTool_STGC_Cfg(flags)
        kwargs.setdefault("STGCRegionSelector", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("STGCRegionSelector", "")

    if flags.Detector.GeometryMM:
        from RegionSelector.RegSelToolConfig import regSelTool_MM_Cfg
        acc = regSelTool_MM_Cfg(flags)
        kwargs.setdefault("MMRegionSelector", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("MMRegionSelector", "")


    acc = MuonTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool", acc.getPrimary())
    result.merge(acc)
    if flags.Common.isOnline:
        kwargs.setdefault("MdtCondKey", "")
        
    segment_region_recovery_tool = Muon__MuonSegmentRegionRecoveryTool(name, **kwargs)
    result.setPrivateTools(segment_region_recovery_tool)
    return result
    
def MuPatCandidateToolCfg(flags, name="MuPatCandidateTool", **kwargs):
    # https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuPatTools.py#L32
    
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_dcot_creator = result.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)

    if flags.Detector.GeometryCSC:
        from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg
        kwargs.setdefault("CscRotCreator", result.popToolsAndMerge(CscClusterOnTrackCreatorCfg(flags)))
    else:
        kwargs.setdefault("CscRotCreator", "")

    kwargs.setdefault("MuonPrinterTool", MuonEDMPrinterTool(flags) )

    from MuonConfig.MuonRecToolsConfig import MuPatHitToolCfg
    kwargs.setdefault("HitTool", result.getPrimaryAndMerge(MuPatHitToolCfg(flags)))

    result.setPrivateTools(CompFactory.Muon.MuPatCandidateTool(name, **kwargs))
    return result
    
def MuonChamberHoleRecoveryToolCfg(flags, name="MuonChamberHoleRecoveryTool", **kwargs):
    result=ComponentAccumulator()
    # Not setting explicitly MuonStationIntersectSvc

    from MuonConfig.MuonRecToolsConfig import MuonExtrapolatorCfg
    acc = MuonExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popToolsAndMerge(acc))

    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg
    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_dcot_creator = acc.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)
    result.merge(acc)

    kwargs.setdefault("AddMeasurements",  not flags.Muon.doSegmentT0Fit )
    if flags.Detector.GeometryCSC:
        extrakwargs={}
        if flags.Muon.enableErrorTuning or not flags.Input.isMC:
            extrakwargs["ErrorScalerBeta"] = 0.200
        from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg
        kwargs.setdefault("CscRotCreator", result.popToolsAndMerge(CscClusterOnTrackCreatorCfg(flags, **extrakwargs)))
    else:
        kwargs["CscRotCreator"] = None
        kwargs.setdefault("CscPrepDataContainer","")
    
    if not flags.Detector.GeometrysTGC:
        kwargs.setdefault("sTgcPrepDataContainer","")
    
    if not flags.Detector.GeometryMM:
        kwargs.setdefault("MMPrepDataContainer","")
    
    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')    
    if flags.Common.isOnline:
        kwargs.setdefault("MdtCondKey","")

    result.setPrivateTools(CompFactory.Muon.MuonChamberHoleRecoveryTool(name, **kwargs))
    return result
    

def MuonTrackSteeringCfg(flags, name="MuonTrackSteering", **kwargs):
    Muon__MuonTrackSteering=CompFactory.Muon.MuonTrackSteering
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
    
    acc = MuonSegmentRegionRecoveryToolCfg(flags, name='MuonEORecoveryTool', OnlyEO = True, Fitter=mctbslfitter, UseFitterOutlierLogic=False)
    muon_eo_recovery_tool = acc.getPrimary()
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
        acc = MooTrackBuilderCfg(flags, name='TrackBuilderTemplate')
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
    kwargs.setdefault("MuPatCandidateTool",       cand_tool) 

    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg
    acc  = MuonAmbiProcessorCfg(flags, name='MuonAmbiProcessor')
    ambi = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("AmbiguityTool",       ambi) 
    
    kwargs.setdefault("MooBuilderTool",       builder) 
    kwargs.setdefault("TrackRefinementTool",       builder) 
    
    acc = MooCandidateMatchingToolCfg(flags, doSegmentPhiMatching=True)
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
    result.merge(acc)
    
    kwargs.setdefault("HoleRecoveryTool",       hole_recovery_tool) 
    
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
    acc = MuonTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool",  acc.getPrimary())
    result.merge(acc)

    track_maker_steering = Muon__MuonTrackSteering(name,**kwargs)
    result.setPrivateTools(track_maker_steering)
    return result

def MuonTrackSelector(flags, name = "MuonTrackSelectorTool", **kwargs):
    Muon__MuonTrackSelectorTool=CompFactory.Muon.MuonTrackSelectorTool
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

def MuonStandaloneTrackParticleCnvAlgCfg(flags, name = "MuonStandaloneTrackParticleCnvAlg", **kwargs):
    from InDetConfig.TrackRecoConfig import TrackCollectionCnvToolCfg, TrackParticleCnvAlgCfg, TrackParticleCreatorToolCfg
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg

    result = MuonTrackSummaryToolCfg(flags)
    muon_particle_creator_tool = result.getPrimaryAndMerge( TrackParticleCreatorToolCfg(flags, name="MuonParticleCreatorTool", 
                                                                    TrackSummaryTool=result.getPrimary(), KeepAllPerigee=True,
                                                                    MuonSummaryTool= CompFactory.Muon.MuonHitSummaryTool("MuonHitSummaryTool"),
                                                                    PerigeeExpression="Origin"  ) )
    track_collection_cnv_tool = result.popToolsAndMerge(TrackCollectionCnvToolCfg(flags, name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muon_particle_creator_tool))
    kwargs.setdefault("TrackParticleCreator",  muon_particle_creator_tool)
    kwargs.setdefault("RecTrackParticleContainerCnvTool", CompFactory.xAODMaker.RecTrackParticleContainerCnvTool(name = "MuonRecTrackParticleContainerCnvTool", TrackParticleCreator = muon_particle_creator_tool))
    kwargs.setdefault("TrackCollectionCnvTool", track_collection_cnv_tool)
    kwargs.setdefault("TrackContainerName", "MuonSpectrometerTracks")
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", "MuonSpectrometerTrackParticles")
    kwargs.setdefault("AODContainerName", "")
    kwargs.setdefault("AODTruthContainerName", "")
    kwargs.setdefault("xAODTruthLinkVector",  "")
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("AddTruthLink", False)

    result.merge( TrackParticleCnvAlgCfg(flags, name,**kwargs) )
    return result

def MuonTrackBuildingCfg(flags, name = "MuPatTrackBuilder", **kwargs):
    MuPatTrackBuilder=CompFactory.MuPatTrackBuilder
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py#L162
    result=ComponentAccumulator()
    
    #Arguably this should be somewhere deeper - not quite sure where yet though. FIXME
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg 
    result.merge( MuonGeoModelCfg(flags) )
    
    Muon__MuonEDMHelperSvc=CompFactory.Muon.MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    result.addService( muon_edm_helper_svc )
    
    acc = MuonTrackSteeringCfg(flags)
    track_steering = acc.getPrimary()
    result.merge(acc)

    # release 21 ESDs contain a Trk::SegmentCollection named 'MuonSegments' instead of 'TrackMuonSegments', the following 2 lines account for that
    from MuonConfig.MuonSegmentNameFixConfig import MuonSegmentNameFixCfg
    result.merge(MuonSegmentNameFixCfg(flags))
    
    track_builder = MuPatTrackBuilder(name=name, TrackSteering = track_steering, MuonSegmentCollection="TrackMuonSegments", SpectrometerTrackOutputLocation="MuonSpectrometerTracks", **kwargs)

    result.addEventAlgo( track_builder, primary=True )
    return result
    

if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonConfig.MuonTrackBuildingConfig --run --threads=
    from MuonConfig.MuonConfigUtils import SetupMuonStandaloneArguments, SetupMuonStandaloneConfigFlags, SetupMuonStandaloneOutput, SetupMuonStandaloneCA

    args = SetupMuonStandaloneArguments()
    ConfigFlags = SetupMuonStandaloneConfigFlags(args)
    cfg = SetupMuonStandaloneCA(args,ConfigFlags)
          
    # Run the actual test.
    acc = MuonTrackBuildingCfg(ConfigFlags)
    cfg.merge(acc)
    
    if args.threads>1 and args.forceclone:
        from AthenaCommon.Logging import log
        log.info('Forcing track building cardinality to be equal to '+str(args.threads))
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        AlgResourcePool=CompFactory.AlgResourcePool
        cfg.addService(AlgResourcePool( OverrideUnClonable=True ) )
        track_builder = acc.getPrimary()
        track_builder.Cardinality=args.threads
            
    # This is a temporary fix - it should go someplace central as it replaces the functionality of addInputRename from here:
    # https://gitlab.cern.ch/atlas/athena/blob/master/Control/SGComps/python/AddressRemappingSvc.py
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    pps = ProxyProviderSvc()
    ars=AddressRemappingSvc()
    pps.ProviderNames += [ 'AddressRemappingSvc' ]
    ars.TypeKeyRenameMaps += [ '%s#%s->%s' % ("TrackCollection", "MuonSpectrometerTracks", "MuonSpectrometerTracks_old") ]
    
    cfg.addService(pps)
    cfg.addService(ars)

    itemsToRecord = ["TrackCollection#MuonSpectrometerTracks"] 
    SetupMuonStandaloneOutput(cfg, ConfigFlags, itemsToRecord)
    
    cfg.printConfig(withDetails = True)
              
    f=open("MuonTrackBuilding.pkl","wb")
    cfg.store(f)
    f.close()
    
    if args.run:
        sc = cfg.run(20)
        if not sc.isSuccess():
            import sys
            sys.exit("Execution failed")
        
