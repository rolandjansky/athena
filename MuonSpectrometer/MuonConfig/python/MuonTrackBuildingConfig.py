# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MuonConfig.MuonRecToolsConfig import MCTBFitterCfg, MuonTrackCleanerCfg, MuonSegmentMomentumFromFieldCfg, MuonSeededSegmentFinderCfg, MuonEDMPrinterToolCfg
from AthenaConfiguration.Enums import BeamType
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFittingToolCfg 

def MooTrackFitterCfg(flags, name = 'MooTrackFitter', **kwargs):
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L179
    # (Tons of explicit configuration missing from there though)
    Muon__MooTrackFitter=CompFactory.Muon.MooTrackFitter
    MuonSegmentMomentum=CompFactory.MuonSegmentMomentum
    from MuonConfig.MuonRecToolsConfig import MuonPhiHitSelector, MuonTrackToSegmentToolCfg, MuonTrackSummaryHelperToolCfg, MuPatHitToolCfg
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg
    
    result = ComponentAccumulator()
    mctb_fitter = result.getPrimaryAndMerge(MCTBFitterCfg(flags))
    kwargs.setdefault("Fitter", mctb_fitter)
        
    mu_pat_hit_tool = result.popToolsAndMerge(MuPatHitToolCfg(flags))
    result.addPublicTool(mu_pat_hit_tool)
    kwargs.setdefault("HitTool", mu_pat_hit_tool)
    
    from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasSTEP_PropagatorCfg
    muon_prop =  result.popToolsAndMerge(AtlasSTEP_PropagatorCfg(flags, name="MuonSTEP_Propagator"))
    
    kwargs.setdefault("Propagator",      muon_prop)
    # kwargs.setdefault("SLFit" ,          ) # Was "not jobproperties.BField.allToroidOn()" but do not have access to Field here.
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("SLFit", False)
    kwargs.setdefault("ReducedChi2Cut",  flags.Muon.Chi2NDofCut)
    
    momentum_estimator=""
    if flags.Beam.Type is BeamType.Cosmics:
        momentum_estimator = result.popToolsAndMerge(MuonSegmentMomentum(DoCosmics = True))
    else:
        momentum_estimator=result.popToolsAndMerge(MuonSegmentMomentumFromFieldCfg(flags))
        
    result.addPublicTool(momentum_estimator)
    kwargs.setdefault("SegmentMomentum", momentum_estimator )
    
    kwargs.setdefault("MuonPrinterTool", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags) ))

    track_to_segment_tool =  result.popToolsAndMerge(MuonTrackToSegmentToolCfg(flags))
    kwargs.setdefault("TrackToSegmentTool", track_to_segment_tool)    
    
    mdt_dcot_creator = result.popToolsAndMerge(MdtDriftCircleOnTrackCreatorCfg(flags))
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)
    
    kwargs.setdefault("PhiHitSelector",  MuonPhiHitSelector(flags))
    
    
    track_cleaner = result.popToolsAndMerge(MuonTrackCleanerCfg(flags))
    result.addPublicTool(track_cleaner)
    kwargs.setdefault("TrackCleaner",  track_cleaner)
    # Leaving "SegmentInOverlapTool" as default, which is what happens in the current configuration.
    
    track_summary_helper = result.popToolsAndMerge(MuonTrackSummaryHelperToolCfg(flags)) 
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
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg
    from MuonConfig.MuonRecToolsConfig import MuonTrackToSegmentToolCfg, MuonTrackExtrapolationToolCfg
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    
    # Based on this: https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L221
    # ignoring all the name_prefix stuff for the moment, since I'm not sure it's necessary any more.
    result = ComponentAccumulator() 
    moo_track_fitter = result.popToolsAndMerge(MooTrackFitterCfg(flags))
    kwargs.setdefault("Fitter", moo_track_fitter)
    
    # Now setup SL fitter (MooSLTrackFitter as defined here:
    # https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecExampleConfigDb.py#L203)
    mctbslfitter = result.popToolsAndMerge(MCTBFitterCfg(flags, name='MCTBSLFitter', StraightLine=True))
    
    # Just take the default configuration, as per https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecExampleConfigDb.py#L56
    from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasSTEP_PropagatorCfg
    prop = result.popToolsAndMerge(AtlasSTEP_PropagatorCfg(flags,name = 'MuonStraightLinePropagator'))
    
    moo_sl_track_fitter = result.popToolsAndMerge(MooTrackFitterCfg( flags, name="MooSLTrackFitter", Fitter = mctbslfitter, Propagator=prop, ReducedChi2Cut=10.0,  SLFit=True))
    
    kwargs.setdefault("SLFitter", moo_sl_track_fitter)
    kwargs.setdefault("RecalibrateMDTHitsOnTrack", ( (not flags.Muon.doSegmentT0Fit) and flags.Beam.Type is BeamType.Collisions) )
    
    muon_seeded_segment_finder = result.popToolsAndMerge(MuonSeededSegmentFinderCfg(flags))
    kwargs.setdefault("SeededSegmentFinder", muon_seeded_segment_finder)

    mdt_dcot_creator = result.popToolsAndMerge(MdtDriftCircleOnTrackCreatorCfg(flags))
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)
    
    muon_comp_cluster_creator =  result.popToolsAndMerge(TriggerChamberClusterOnTrackCreatorCfg(flags))
    kwargs.setdefault("CompetingClustersCreator", muon_comp_cluster_creator)    
    
    from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasSTEP_PropagatorCfg
    muon_prop = result.popToolsAndMerge(AtlasSTEP_PropagatorCfg(flags, name="MuonSTEP_Propagator"))
    kwargs.setdefault("Propagator", muon_prop)    
    
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
    
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags)))

    kwargs.setdefault('Extrapolator', result.popToolsAndMerge( MuonTrackExtrapolationToolCfg(flags) ) )

    # FIXME - remove ErrorOptimisationTool from cxx?
    # declareProperty("ErrorOptimisationTool","" );Extrapolator

    acc=MuPatCandidateToolCfg(flags)
    cand_tool = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CandidateTool",       cand_tool)

    kwargs.setdefault("CandidateMatchingTool", 
                      result.popToolsAndMerge(MooCandidateMatchingToolCfg(flags)))

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
    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("OverlapMatchAveragePhiHitPullCut",  200.)
        kwargs.setdefault( "ToroidOn", False ) # Was "not jobproperties.BField.allToroidOn()" but do not have access to Field here.

    # There are two tools which this depends on which aren't properties and which weren't defined in old configuration
    # Ignore for now, but FIXME one day
    # m_overlapResolvingTool("Muon::MuonSegmentInOverlapResolvingTool/MuonSegmentInOverlapResolvingTool"),
    # m_pairMatchingTool("Muon::MuonSegmentPairMatchingTool/MuonSegmentPairMatchingTool"),
    # Also, residual pull calculator not yet configured. #FIXME
    
    result = MuonEDMPrinterToolCfg(flags)
    kwargs.setdefault("Printer", result.getPrimary() )

    matching = Muon__MuonSegmentMatchingTool(name, **kwargs)
    result.setPrivateTools(matching)
    return result
    
def MooCandidateMatchingToolCfg(flags, name="MooCandidateMatchingTool", doSegmentPhiMatching=False, **kwargs):
    Muon__MooCandidateMatchingTool=CompFactory.Muon.MooCandidateMatchingTool
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    
    result = ComponentAccumulator()

    # Won't explicitly configure MuonEDMHelperSvc
    kwargs.setdefault("MuonPrinterTool", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags) ))
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
    kwargs.setdefault("RequireSameSide", flags.Beam.Type is not BeamType.Collisions)
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
    from TrkConfig.AtlasExtrapolatorConfig import MuonExtrapolatorCfg
    from MuonConfig.MuonRecToolsConfig import MuonStationIntersectCondAlgCfg, MuonTrackSummaryToolCfg
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L426
    
    result = ComponentAccumulator()
    kwargs.setdefault("SeededSegmentFinder", 
                       result.popToolsAndMerge(MuonSeededSegmentFinderCfg(flags)))
    
    kwargs.setdefault("TrackSegmentMatchingTool", 
                      result.popToolsAndMerge(MooCandidateMatchingToolCfg(flags)))
    
    kwargs.setdefault("ChamberHoleRecoveryTool", 
                      result.popToolsAndMerge(MuonChamberHoleRecoveryToolCfg(flags)))
    
    kwargs.setdefault("Extrapolator", 
                      result.popToolsAndMerge(MuonExtrapolatorCfg(flags)))
    
    kwargs.setdefault("Fitter", 
                      result.popToolsAndMerge(MCTBFitterCfg(flags, name='MCTBSLFitter', StraightLine=True)))
    
    result.merge( MuonStationIntersectCondAlgCfg(flags))
    
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
        
    segment_region_recovery_tool = Muon__MuonSegmentRegionRecoveryTool(name, **kwargs)
    result.setPrivateTools(segment_region_recovery_tool)
    return result
    
def MuPatCandidateToolCfg(flags, name="MuPatCandidateTool", **kwargs):
    # https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuPatTools.py#L32
    
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_dcot_creator = result.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)

    if flags.Detector.GeometryCSC:
        from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import CscClusterOnTrackCreatorCfg
        kwargs.setdefault("CscRotCreator", result.popToolsAndMerge(CscClusterOnTrackCreatorCfg(flags)))
    else:
        kwargs.setdefault("CscRotCreator", "")

    kwargs.setdefault("MuonPrinterTool", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags) ))

    from MuonConfig.MuonRecToolsConfig import MuPatHitToolCfg
    kwargs.setdefault("HitTool", result.getPrimaryAndMerge(MuPatHitToolCfg(flags)))

    result.setPrivateTools(CompFactory.Muon.MuPatCandidateTool(name, **kwargs))
    return result
    
def MuonChamberHoleRecoveryToolCfg(flags, name="MuonChamberHoleRecoveryTool", **kwargs):
    result=ComponentAccumulator()
    # Not setting explicitly MuonStationIntersectSvc

    from TrkConfig.AtlasExtrapolatorConfig import MuonExtrapolatorCfg
    acc = MuonExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popToolsAndMerge(acc))

    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg
    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_dcot_creator = acc.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_dcot_creator)
    result.merge(acc)

    kwargs.setdefault("AddMeasurements",  not flags.Muon.doSegmentT0Fit )
    if flags.Detector.GeometryCSC:
        extrakwargs={}
        if flags.Muon.enableErrorTuning or not flags.Input.isMC:
            extrakwargs["ErrorScalerBeta"] = 0.200
        from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import CscClusterOnTrackCreatorCfg
        kwargs.setdefault("CscRotCreator", result.popToolsAndMerge(CscClusterOnTrackCreatorCfg(flags, **extrakwargs)))
    else:
        kwargs["CscRotCreator"] = None
        kwargs.setdefault("CscPrepDataContainer","")
    
    if not flags.Detector.GeometrysTGC:
        kwargs.setdefault("sTgcPrepDataContainer","")
    
    if not flags.Detector.GeometryMM:
        kwargs.setdefault("MMPrepDataContainer","")
    
    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')    
    kwargs.setdefault("EDMPrinter", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags) ))

    result.setPrivateTools(CompFactory.Muon.MuonChamberHoleRecoveryTool(name, **kwargs))
    return result

def EMEO_MuonChamberHoleRecoveryToolCfg(flags, name = "MuonChamberRecovery_EMEO"):   
    return MuonChamberHoleRecoveryToolCfg(flags,
                                         name=name,
                                         sTgcPrepDataContainer="",
                                         MMPrepDataContainer="")

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
    else: builder = kwargs["TrackBuilderTool"]
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
    
    kwargs.setdefault("CandidateMatchingTool", 
        result.popToolsAndMerge(MooCandidateMatchingToolCfg(flags, doSegmentPhiMatching=True)))
    
    kwargs.setdefault("MuonSegmentFittingTool",       
                       result.popToolsAndMerge(MuonSegmentFittingToolCfg(flags))) 
    kwargs.setdefault("MuonTrackSelector", 
                       result.popToolsAndMerge(MuonTrackSelectorCfg(flags)))
   
    kwargs.setdefault("HoleRecoveryTool",       
                      result.popToolsAndMerge(MuonChamberHoleRecoveryToolCfg(flags))) 
    
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
    acc = MuonTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool",  acc.getPrimary())
    result.merge(acc)

    track_maker_steering = Muon__MuonTrackSteering(name,**kwargs)
    result.setPrivateTools(track_maker_steering)
    return result

def MuonTrackSelectorCfg(flags, name = "MuonTrackSelectorTool", **kwargs):
    result = ComponentAccumulator()
    if flags.Beam.Type in [BeamType.Cosmics, BeamType.SingleBeam] or not flags.Input.isMC:
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
    
    theTool = CompFactory.Muon.MuonTrackSelectorTool(name, **kwargs)
    result.setPrivateTools(theTool)
    return result

def MuonStandaloneTrackParticleCnvAlgCfg(flags, name = "MuonStandaloneTrackParticleCnvAlg", **kwargs):
    from InDetConfig.TrackRecoConfig import TrackCollectionCnvToolCfg, TrackParticleCnvAlgCfg, TrackParticleCreatorToolCfg, RecTrackParticleContainerCnvToolCfg
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg, MuonHitSummaryToolCfg

    result = ComponentAccumulator()
    track_summary_tool = result.getPrimaryAndMerge(MuonTrackSummaryToolCfg(flags))
    muon_hit_summary_tool = result.popToolsAndMerge(MuonHitSummaryToolCfg(flags))
    muon_particle_creator_tool = result.getPrimaryAndMerge( TrackParticleCreatorToolCfg(flags, name="MuonParticleCreatorTool", 
                                                                    TrackSummaryTool=track_summary_tool, 
                                                                    KeepAllPerigee=True,
                                                                    MuonSummaryTool= muon_hit_summary_tool,
                                                                    PerigeeExpression="Origin"  ) )
    track_collection_cnv_tool = result.getPrimaryAndMerge(TrackCollectionCnvToolCfg(flags, name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muon_particle_creator_tool))
    kwargs.setdefault("TrackParticleCreator",  muon_particle_creator_tool)
    kwargs.setdefault("RecTrackParticleContainerCnvTool", result.getPrimaryAndMerge(RecTrackParticleContainerCnvToolCfg(flags,
                                                                                                                        name = "MuonRecTrackParticleContainerCnvTool", 
                                                                                                                        TrackParticleCreator = muon_particle_creator_tool)))
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

def EMEO_MuPatTrackBuilderCfg(flags):
    result = ComponentAccumulator()

    recovery_tool = result.getPrimaryAndMerge(EMEO_MuonChamberHoleRecoveryToolCfg(flags))
    acc = MooTrackBuilderCfg(flags, 
                             name = "MooMuonTrackBuilder_EMEO",
                             ChamberHoleRecoveryTool = recovery_tool)
    
    track_builder= result.getPrimaryAndMerge(acc)

    acc = MuonTrackSteeringCfg(flags,
                               name = "MuonTrackSteering_EMEO",
                               TrackBuilderTool = track_builder)
    track_steering = acc.getPrimary()
    result.merge(acc)
    the_alg = CompFactory.MuPatTrackBuilder(name = "MuPatTrackBuilder_EMEO",
                                            TrackSteering=track_steering, 
                                            SpectrometerTrackOutputLocation="EMEO_MuonSpectrometerTracks", 
                                            MuonSegmentCollection="TrackMuonSegmentsEMEO")
    result.addEventAlgo(the_alg, primary = True)
    return result

def MuPatTrackBuilderCfg(flags, name = "MuPatTrackBuilder", **kwargs):
    result=ComponentAccumulator()
    acc = MuonTrackSteeringCfg(flags)
    track_steering = acc.getPrimary()
    result.merge(acc)
    
    track_builder = CompFactory.MuPatTrackBuilder(name=name, 
                                                TrackSteering = track_steering, 
                                                MuonSegmentCollection="TrackMuonSegments", 
                                                SpectrometerTrackOutputLocation="MuonSpectrometerTracks", **kwargs)
    result.addEventAlgo( track_builder, primary=True )
   
    return result
def MuonTrackBuildingCfg(flags, name = "MuPatTrackBuilder", **kwargs):    
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.3/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py#L162
    result=ComponentAccumulator()
    
    #Arguably this should be somewhere deeper - not quite sure where yet though. FIXME
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg 
    result.merge( MuonGeoModelCfg(flags) )
    
    Muon__MuonEDMHelperSvc=CompFactory.Muon.MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    result.addService( muon_edm_helper_svc )
    
    

    # release 21 ESDs contain a Trk::SegmentCollection named 'MuonSegments' instead of 'TrackMuonSegments', the following 2 lines account for that
    from MuonConfig.MuonSegmentNameFixConfig import MuonSegmentNameFixCfg
    result.merge(MuonSegmentNameFixCfg(flags))    
    result.merge(MuPatTrackBuilderCfg(flags))
    
    if flags.Muon.runCommissioningChain:
        result.merge(EMEO_MuPatTrackBuilderCfg(flags))      
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
        
