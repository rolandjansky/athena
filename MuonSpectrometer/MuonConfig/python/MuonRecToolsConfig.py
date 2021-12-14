# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Configuration of tools shared between Segment Finding and Track Building

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonEDMPrinterTool(flags, name="MuonEDMPrinterTool", **kwargs):
    kwargs.setdefault('TgcPrdCollection', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    return CompFactory.Muon.MuonEDMPrinterTool(name, **kwargs)

def MuonTrackToSegmentToolCfg(flags,name="MuonTrackToSegmentTool", **kwargs):
    Muon__MuonTrackToSegmentTool=CompFactory.Muon.MuonTrackToSegmentTool
    #MDT conditions information not available online
    if flags.Common.isOnline:
        kwargs.setdefault("MdtCondKey","")
    
    result = MuonStationIntersectSvcCfg(flags)
    msis = result.getPrimary()
    kwargs.setdefault("MuonStationIntersectSvc",msis)
    
    # FIXME - this should have a CA
    RkPropagator=CompFactory.Trk.RungeKuttaPropagator
    
    atlasRungeKuttaPropagator = RkPropagator(name = 'AtlasRungeKuttaPropagator')
    result.addPublicTool(atlasRungeKuttaPropagator)
    kwargs.setdefault("Propagator",atlasRungeKuttaPropagator)
    
    # Not bothering to explicitly set IdHelper or EDMHelper    
    muon_track_to_segment_tool = Muon__MuonTrackToSegmentTool(name, **kwargs)
    result.setPrivateTools(muon_track_to_segment_tool)
    return result


def MuonSeededSegmentFinderCfg(flags,name="MuonSeededSegmentFinder", **kwargs):
    Muon__MuonSeededSegmentFinder=CompFactory.Muon.MuonSeededSegmentFinder
    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MdtMathSegmentFinderCfg # FIXME - should really shift this to RecTools then.
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg
    result = ComponentAccumulator()
    
    mdt_segment_finder = result.popToolsAndMerge( MdtMathSegmentFinderCfg(flags, name="MCTBMdtMathSegmentFinder", UseChamberTheta = False, AssociationRoadWidth = 1.5) )
    result.addPublicTool(mdt_segment_finder)
    
    if "SegmentMaker" not in kwargs or "SegmentMakerNoHoles" not in kwargs:
        seg_maker=""
        acc={}
        if flags.Beam.Type == 'collisions':
            acc = DCMathSegmentMakerCfg( flags, name = "MCTBDCMathSegmentMaker", MdtSegmentFinder = mdt_segment_finder, SinAngleCut = 0.04, DoGeometry = True)
        else:  # cosmics or singlebeam
            acc = DCMathSegmentMakerCfg( flags, name = "MCTBDCMathSegmentMaker", MdtSegmentFinder = mdt_segment_finder, SinAngleCut = 0.1,  DoGeometry = False, AddUnassociatedPhiHits= True )
        seg_maker = acc.getPrimary()
        result.merge(acc)
        kwargs.setdefault("SegmentMaker", seg_maker)
        kwargs.setdefault("SegmentMakerNoHoles", seg_maker) #FIXME. Just remove one.
    if not flags.Detector.GeometryCSC:
        kwargs.setdefault("CscPrepDataContainer","")
    if not flags.Detector.GeometrysTGC:
        kwargs.setdefault("sTgcPrepDataContainer","")
    if not flags.Detector.GeometryMM:
        kwargs.setdefault("MMPrepDataContainer","")
    
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )
    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtRotCreator", acc.getPrimary())
    result.merge(acc)

    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    
    muon_seeded_segment_finder = Muon__MuonSeededSegmentFinder(name, **kwargs)
    result.setPrivateTools(muon_seeded_segment_finder)
    return result
        
        
def MuonSegmentMomentumFromFieldCfg(flags, name="MuonSegmentMomentumFromField", **kwargs):
    MuonSegmentMomentumFromField=CompFactory.MuonSegmentMomentumFromField
    result = ComponentAccumulator()
    
    navigator_ca = MuonNavigatorCfg(flags)
    navigator = navigator_ca.popPrivateTools()
    result.addPublicTool(navigator)
    result.merge(navigator_ca)
    kwargs.setdefault("NavigatorTool", navigator)
    
    acc = MuonSTEP_PropagatorCfg(flags)
    muon_prop = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("PropagatorTool", muon_prop)
        
    muon_seg_mom_from_field = MuonSegmentMomentumFromField(name=name, **kwargs)
    result.setPrivateTools(muon_seg_mom_from_field)
    return result
    
def MuonTrackSummaryHelperToolCfg(flags, name="MuonTrackSummaryHelperTool", **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    
    result = AtlasExtrapolatorCfg(flags)
    extrap = result.getPrimary()
    result.addPublicTool(extrap)
    kwargs.setdefault("Extrapolator", extrap)

    kwargs.setdefault("CalculateCloseHits", True)

    Muon__MuonTrackSummaryHelperTool=CompFactory.Muon.MuonTrackSummaryHelperTool
    result.setPrivateTools(Muon__MuonTrackSummaryHelperTool(name=name,**kwargs))
    return result

def MuonTrackSummaryToolCfg(flags, name="MuonTrackSummaryTool", **kwargs):
    Trk__TrackSummaryTool=CompFactory.Trk.TrackSummaryTool
    
    result = ComponentAccumulator()
    acc = MuonTrackSummaryHelperToolCfg(flags)
    track_summary_helper = acc.getPrimary()
    acc.addPublicTool(track_summary_helper)
    result.merge(acc)
    kwargs.setdefault("MuonSummaryHelperTool", track_summary_helper )
    kwargs.setdefault("doSharedHits", False )
    kwargs.setdefault("AddDetailedMuonSummary", True )
    result.setPrivateTools(Trk__TrackSummaryTool(name=name,**kwargs))
    return result

def MuonTrackScoringToolCfg(flags, name="MuonTrackScoringTool", **kwargs):
    Muon__MuonTrackScoringTool=CompFactory.Muon.MuonTrackScoringTool
    
    # m_trkSummaryTool("Trk::TrackSummaryTool"),    
    # m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    result = ComponentAccumulator()
    acc = MuonTrackSummaryToolCfg(flags)
    track_summary = acc.getPrimary( )
    acc.addPublicTool(track_summary)
    result.merge(acc)
    kwargs.setdefault('SumHelpTool', track_summary)
    result.setPrivateTools(Muon__MuonTrackScoringTool(name=name,**kwargs))
    return result

def MuonAmbiProcessorCfg(flags, name="MuonAmbiProcessor", **kwargs):
    Trk__TrackSelectionProcessorTool=CompFactory.Trk.TrackSelectionProcessorTool
    Muon__MuonAmbiTrackSelectionTool=CompFactory.Muon.MuonAmbiTrackSelectionTool
    
    #m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
    #m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool")
    result = ComponentAccumulator()
    kwargs.setdefault('DropDouble', False)
    acc  = MuonTrackScoringToolCfg( flags )
    scoring_tool = acc.getPrimary()
    acc.addPublicTool(scoring_tool)
    result.merge(acc)
    kwargs.setdefault('ScoringTool', scoring_tool )
    muon_ami_selection_tool = Muon__MuonAmbiTrackSelectionTool(name="MuonAmbiSelectionTool")
    result.addPublicTool(muon_ami_selection_tool)
    kwargs.setdefault('SelectionTool', muon_ami_selection_tool)
    result.setPrivateTools(Trk__TrackSelectionProcessorTool(name=name,**kwargs))
    return result

def MuonTrackCleanerCfg(flags, name="MuonTrackCleaner", **kwargs):
    Muon__MuonTrackCleaner=CompFactory.Muon.MuonTrackCleaner
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    # declareProperty("Helper",m_edmHelperSvc);
    # declareProperty("Printer",m_printer);
    # declareProperty("MdtRotCreator",  m_mdtRotCreator );
    # declareProperty("CompRotCreator", m_compRotCreator );
    # declareProperty("PullCalculator", m_pullCalculator );
    # declareProperty("Extrapolator",   m_extrapolator );
    # declareProperty("MagFieldSvc",    m_magFieldSvc );
    # declareProperty("MeasurementUpdator",   m_measurementUpdator );
    result=ComponentAccumulator()
    
    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MdtRotCreator", mdt_creator)
    
    acc = TriggerChamberClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CompRotCreator", muon_cluster_creator)
    
    # For PullCalculator, just let it get default for moment. FIXME
    
    extrapolator_CA = AtlasExtrapolatorCfg(flags)
    extrapolator = extrapolator_CA.getPrimary()
    acc.addPublicTool( extrapolator ) # TODO remove
    result.merge( extrapolator_CA )
    kwargs.setdefault("Extrapolator", extrapolator)

    acc = MCTBSLFitterMaterialFromTrackCfg(flags)
    slfitter = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)

    acc = MCTBFitterCfg(flags, name = "MCTBFitterMaterialFromTrack", GetMaterialFromTrack=True)
    fitter = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("Fitter", fitter)

    # kwargs.setdefault("MagFieldSvc", mag_field_svc) Default for moment
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )

    kwargs.setdefault("MaxAvePullSumPerChamber", 6)
    kwargs.setdefault("Chi2Cut", flags.Muon.Chi2NDofCut)
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)

    # FIXME - do remaining tools

    
    result.setPrivateTools(Muon__MuonTrackCleaner(name, **kwargs))
    
    return result 

def MuonStationIntersectSvcCfg(flags, name='MuonStationIntersectSvc',**kwargs):
    MuonStationIntersectSvc=CompFactory.MuonStationIntersectSvc
    # Has dependency IdHelperTool (which we ignore for now)
    result = ComponentAccumulator()
    muon_station_intersect_svc = MuonStationIntersectSvc(name=name, **kwargs)
    result.addService(muon_station_intersect_svc, primary=True)
    return result

# default muon navigator
def MuonNavigatorCfg(flags, name="MuonNavigator", **kwargs):
    Trk__Navigator = CompFactory.Trk.Navigator
    result = ComponentAccumulator()
    
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    result.merge(acc)
    kwargs.setdefault("TrackingGeometryKey", geom_cond_key)

    navigator = Trk__Navigator(name=name, **kwargs)
    result.setPrivateTools(navigator)
    return result

def MuonStraightLineExtrapolatorCfg(flags, name="MuonStraightLineExtrapolator",**kwargs):
    # This is a bit odd , but this is exactly what was in the old configuration
    acc = MuonSTEP_PropagatorCfg(flags, name = "MuonStraightLinePropagator")
    muon_prop = acc.getPrimary()
    kwargs.setdefault("Propagators",[ muon_prop])
    kwargs.setdefault("STEP_Propagator",muon_prop)
    result = MuonExtrapolatorCfg(flags, name ,**kwargs)
    result.merge (acc)
    return result

def MuonExtrapolatorCfg(flags,name = "MuonExtrapolator", **kwargs):
    Trk__MaterialEffectsUpdator, Trk__EnergyLossUpdator, Trk__MultipleScatteringUpdator=CompFactory.getComps("Trk::MaterialEffectsUpdator","Trk::EnergyLossUpdator","Trk::MultipleScatteringUpdator",)
    
    Trk__Extrapolator=CompFactory.Trk.Extrapolator
    result = ComponentAccumulator()
    
    energy_loss_updator = Trk__EnergyLossUpdator(name="AtlasEnergyLossUpdator") # Not really sure these should be tools...
    result.addPublicTool(energy_loss_updator) # TODO remove 

    # This one has a dependency on RndNumberService
    mult_scat_updator = Trk__MultipleScatteringUpdator(name="AtlasMultipleScatteringUpdator")
    result.addPublicTool(mult_scat_updator) # TODO remove 
    
    material_effects_updator = Trk__MaterialEffectsUpdator( name="MuonMaterialEffectsUpdator", EnergyLossUpdator=energy_loss_updator, MultipleScatteringUpdator=mult_scat_updator)
    result.addPublicTool(material_effects_updator)
    kwargs.setdefault("MaterialEffectsUpdators", [material_effects_updator])
    
    navigator_ca = MuonNavigatorCfg(flags, name="InDetNavigator") # FIXME, misnamed to match old-style config. Remove name at some point.
    navigator = navigator_ca.popPrivateTools()
    result.addPublicTool(navigator)
    result.merge(navigator_ca)
    kwargs.setdefault("Navigator", navigator)
    
    if 'Propagators' not in kwargs:
        acc = MuonSTEP_PropagatorCfg(flags, name="MuonPropagator", Tolerance = 0.00001, MaterialEffects=True, IncludeBgradients=True)
        muon_prop = acc.getPrimary()
        result.merge(acc)
        result.addPublicTool(muon_prop)
        kwargs.setdefault("Propagators", [muon_prop])
    
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("Tolerance", 0.0011)  # must be > 1um to avoid missing MTG intersections
    extrap = Trk__Extrapolator(name=name, **kwargs)
    result.setPrivateTools(extrap)
    result.addPublicTool(extrap)
    return result

def MuonChi2TrackFitterCfg(flags, name='MuonChi2TrackFitter', **kwargs):
    Trk__KalmanUpdator=CompFactory.Trk.KalmanUpdator
    Trk__GlobalChi2Fitter=CompFactory.Trk.GlobalChi2Fitter
    import MuonConfig.MuonRIO_OnTrackCreatorConfig # Trying to avoid circular dependencies here
    
    result = ComponentAccumulator()

    extrapolator_CA = MuonExtrapolatorCfg(flags)
    extrapolator= extrapolator_CA.getPrimary()
    result.addPublicTool(extrapolator) # TODO remove
    result.merge(extrapolator_CA)
    
    acc = MuonConfig.MuonRIO_OnTrackCreatorConfig.MuonRotCreatorCfg(flags)
    rotcreator=acc.getPrimary()
    result.merge(acc)
    
    measurement_updater = Trk__KalmanUpdator(name="MuonMeasUpdator")

    kwargs.setdefault("ExtrapolationTool"    , extrapolator)
    kwargs.setdefault("RotCreatorTool"       , rotcreator)
    kwargs.setdefault("MeasurementUpdateTool", measurement_updater)
    kwargs.setdefault("StraightLine"         , False)
    kwargs.setdefault("OutlierCut"           , 3.0)
    kwargs.setdefault("GetMaterialFromTrack" , False)
    kwargs.setdefault("RejectLargeNScat"     , True)

    # take propagator and navigator from the extrapolator
    kwargs.setdefault("PropagatorTool", extrapolator.Propagators[0])
    kwargs.setdefault("NavigatorTool",  extrapolator.Navigator)
    ### We need to include the tracking geometry conditions 
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    result.merge(TrackingGeometryCondAlgCfg(flags))
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)
    fitter = Trk__GlobalChi2Fitter(name=name, **kwargs)
    result.setPrivateTools(fitter)
    # print fitter
    return result
    
def MuonSTEP_PropagatorCfg(flags, name='MuonSTEP_Propagator', **kwargs):
    # Really there should be a central configuration for the STEP propagator. FIXME
    # In the old ConfigDb this was named MuonStraightLinePropagator (!)
    Trk__STEP_Propagator=CompFactory.Trk.STEP_Propagator
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result = ComponentAccumulator()
    
    acc  = MagneticFieldSvcCfg(flags) 
    result.merge(acc)
    
    kwargs.setdefault("Tolerance", 0.00001 )
    kwargs.setdefault("MaterialEffects", True  )
    kwargs.setdefault("IncludeBgradients", True  )
    
    propagator = Trk__STEP_Propagator(name=name, **kwargs)
    result.setPrivateTools(propagator)
    return result

def MCTBExtrapolatorCfg(flags, name='MCTBExtrapolator',**kwargs):
    result = ComponentAccumulator()
    
    acc = MuonSTEP_PropagatorCfg(flags, name = "MCTBPropagator")
    prop = acc.getPrimary()
    result.addPublicTool(prop) 
    result.merge(acc)
    kwargs.setdefault("Propagators", [ prop ]) 
    kwargs.setdefault("ResolveMuonStation", False)
    acc = MuonExtrapolatorCfg(flags, name=name, **kwargs)
    result.setPrivateTools(result.getPrimaryAndMerge(acc))
    
    return result

def MCTBSLFitterMaterialFromTrackCfg(flags, name='MCTBSLFitterMaterialFromTrack', **kwargs):
    kwargs["StraightLine"] = True       # always set
    kwargs["GetMaterialFromTrack"]=True # always set
    acc = MuonStraightLineExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolationTool", acc.getPrimary())

    propagator = CompFactory.Trk.RungeKuttaPropagator(name="MuonRK_Propagator",AccuracyParameter=0.0002)
    kwargs["PropagatorTool"]=propagator

    result = MCTBFitterCfg(flags, name, **kwargs)
    result.merge(acc)
    return result

def MCTBFitterCfg(flags, name='MCTBFitter', **kwargs):
    # didn't bother with MCTBSLFitter, since this seems redundant. Just set "StraightLine" = True since the kwargs are passed on to MuonChi2TrackFitterCfg  
    # Ditto with MCTBFitterMaterialFromTrack. Just set "GetMaterialFromTrack" = True
    result = ComponentAccumulator()
    
    acc = MCTBExtrapolatorCfg(flags)
    mctbExtrapolator = acc.getPrimary()
    result.merge(acc)
    
    kwargs.setdefault("ExtrapolationTool", mctbExtrapolator)
    kwargs.setdefault("GetMaterialFromTrack", True)
    kwargs.setdefault("Momentum", flags.Muon.straightLineFitMomentum)
    
    acc = MuonChi2TrackFitterCfg(flags, name=name, **kwargs)
    mctbfitter = acc.getPrimary()
    result.merge(acc)

    result.setPrivateTools(mctbfitter)
    return result

def MuonPhiHitSelector(flags, name="MuonPhiHitSelector",**kwargs):
    MuonPhiHitSelector=CompFactory.MuonPhiHitSelector
    kwargs.setdefault("MakeClusters", True)
    kwargs.setdefault("CompetingRios", True)
    kwargs.setdefault("DoCosmics", flags.Beam.Type == 'cosmics')

    return MuonPhiHitSelector(name,**kwargs)


def MuPatHitToolCfg(flags, name="MuPatHitTool",**kwargs):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_creator=result.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_creator)

    if flags.Detector.GeometryCSC:
        from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg
        kwargs.setdefault("CscRotCreator", result.popToolsAndMerge(CscClusterOnTrackCreatorCfg(flags)))
    else:
        kwargs.setdefault("CscRotCreator", "")

    result.setPrivateTools(CompFactory.Muon.MuPatHitTool(name,**kwargs))
    return result


def MuonTrackExtrapolationToolCfg(flags, name="MuonTrackExtrapolationTool", **kwargs):
    # FIXME - it seems like this tool needs a lot of configuration still.
    # But perhaps it can be simplified first?
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    result = ComponentAccumulator()
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    result.merge(acc)
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)
    kwargs.setdefault("Cosmics", flags.Beam.Type == 'cosmics')

    kwargs.setdefault("AtlasExtrapolator", result.popToolsAndMerge( AtlasExtrapolatorCfg(flags) ) )
    kwargs.setdefault("MuonExtrapolator", result.getPrimaryAndMerge( MuonExtrapolatorCfg(flags) ) )
    kwargs.setdefault("MuonExtrapolator2", result.getPrimaryAndMerge( MuonExtrapolatorCfg(flags) ) ) # Reported in ATLASRECTS-6658

    result.setPrivateTools(
        CompFactory.Muon.MuonTrackExtrapolationTool(name, **kwargs))
    return result

def MuonRefitToolCfg(flags, name="MuonRefitTool", **kwargs):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg

    # FIXME - many tools are not yet explicitly configured here.
    result= MCTBFitterCfg(flags, name = "MCTBFitterMaterialFromTrack", GetMaterialFromTrack=True)
    kwargs.setdefault("Fitter", result.getPrimary())
    kwargs.setdefault("MuonExtrapolator", result.popToolsAndMerge( MuonExtrapolatorCfg(flags) ) )
    kwargs.setdefault("MdtRotCreator", result.popToolsAndMerge( MdtDriftCircleOnTrackCreatorCfg(flags) ) )
    kwargs.setdefault("CompClusterCreator", result.popToolsAndMerge( TriggerChamberClusterOnTrackCreatorCfg(flags) ) )
    kwargs.setdefault("MuonEntryExtrapolationTool", result.popToolsAndMerge(MuonTrackExtrapolationToolCfg(flags)) )
    if flags.IOVDb.DatabaseInstance == 'COMP200' or \
                'HLT'  in flags.IOVDb.GlobalTag or flags.Common.isOnline or flags.Muon.MuonTrigger:
        kwargs["AlignmentErrorTool"] = None
    result.setPrivateTools(CompFactory.Muon.MuonRefitTool(name, **kwargs))
    return result

