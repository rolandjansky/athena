# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Configuration of tools shared between Segment Finding and Track Building

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Tracking
from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg

def MuonEDMPrinterTool(flags, name="MuonEDMPrinterTool", **kwargs):
    kwargs.setdefault('TgcPrdCollection', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC and not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    return CompFactory.Muon.MuonEDMPrinterTool(name, **kwargs)

def MuonTrackToSegmentToolCfg(flags,name="MuonTrackToSegmentTool", **kwargs):
    Muon__MuonTrackToSegmentTool=CompFactory.Muon.MuonTrackToSegmentTool
    #MDT conditions information not available online
    if(flags.Common.isOnline):
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
    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MdtMathSegmentFinder # FIXME - should really shift this to RecTools then.
    result = ComponentAccumulator()
    
    mdt_segment_finder = MdtMathSegmentFinder(flags, name="MCTBMdtMathSegmentFinder", UseChamberTheta = False, AssociationRoadWidth = 1.5)
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

    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC and not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    
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

    result = ComponentAccumulator()
    acc  = TrackingGeometrySvcCfg(flags)
    
    result.merge(acc)
    
    acc = MuonExtrapolatorCfg(flags)
    extrap = acc.getPrimary()
    acc.addPublicTool(extrap)
    result.merge(acc)
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
    muon_ami_selection_tool = Muon__MuonAmbiTrackSelectionTool()
    result.addPublicTool(muon_ami_selection_tool)
    kwargs.setdefault('SelectionTool', muon_ami_selection_tool)
    result.setPrivateTools(Trk__TrackSelectionProcessorTool(name=name,**kwargs))
    return result

def MuonTrackCleanerCfg(flags, name="MuonTrackCleaner", **kwargs):
    Muon__MuonTrackCleaner=CompFactory.Muon.MuonTrackCleaner
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg
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
    
    extrapolator_CA = MuonExtrapolatorCfg(flags)
    extrapolator = extrapolator_CA.getPrimary()
    acc.addPublicTool( extrapolator ) # TODO remove
    result.merge( extrapolator_CA )
    kwargs.setdefault("Extrapolator", extrapolator)

    acc = MCTBFitterCfg(flags, name = "MCTBSLFitterMaterialFromTrack", StraightLine=True, GetMaterialFromTrack=True)
    slfitter = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)

    acc = MCTBFitterCfg(flags, name = "MCTBFitterMaterialFromTrack", GetMaterialFromTrack=True)
    fitter = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("Fitter", fitter)

    # kwargs.setdefault("MagFieldSvc", mag_field_svc) Default for moment
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )

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
    Trk__Navigator=CompFactory.Trk.Navigator
    
    result = ComponentAccumulator()

    acc  = TrackingGeometrySvcCfg(flags)
    result.merge(acc)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary() )

    navigator = Trk__Navigator(name=name, **kwargs)
    result.setPrivateTools(navigator)   
    return result     

def MuonExtrapolatorCfg(flags,name = "MuonExtrapolator", **kwargs):
    Trk__MaterialEffectsUpdator, Trk__EnergyLossUpdator, Trk__MultipleScatteringUpdator=CompFactory.getComps("Trk::MaterialEffectsUpdator","Trk::EnergyLossUpdator","Trk::MultipleScatteringUpdator",)
    
    Trk__Extrapolator=CompFactory.Trk.Extrapolator
    result = ComponentAccumulator()
    
    energy_loss_updator = Trk__EnergyLossUpdator() # Not really sure these should be tools...
    result.addPublicTool(energy_loss_updator) # TODO remove 

    # This one has a dependency on RndNumberService
    mult_scat_updator = Trk__MultipleScatteringUpdator()
    result.addPublicTool(mult_scat_updator) # TODO remove 
    
    material_effects_updator = Trk__MaterialEffectsUpdator( EnergyLossUpdator=energy_loss_updator, MultipleScatteringUpdator=mult_scat_updator)
    result.addPublicTool(material_effects_updator)
    kwargs.setdefault("MaterialEffectsUpdators", [material_effects_updator])
    
    navigator_ca = MuonNavigatorCfg(flags)
    navigator = navigator_ca.popPrivateTools()
    result.addPublicTool(navigator)
    result.merge(navigator_ca)
    kwargs.setdefault("Navigator", navigator)
    
    if 'Propagators' not in kwargs:
        acc = MuonSTEP_PropagatorCfg(flags, Tolerance = 0.00001, MaterialEffects=True, IncludeBgradients=True)
        muon_prop = acc.getPrimary()
        result.merge(acc)
        result.addPublicTool(muon_prop)
    
    kwargs.setdefault("Propagators", [muon_prop])
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("Tolerance", 0.0011)  # must be > 1um to avoid missing MTG intersections
    extrap = Trk__Extrapolator(name=name, **kwargs)
    result.setPrivateTools(extrap)
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
    
    measurement_updater = Trk__KalmanUpdator()
    result.addPublicTool(measurement_updater) #FIXME

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
    
    acc = MuonSTEP_PropagatorCfg(flags)
    prop = acc.getPrimary()
    result.addPublicTool(prop) 
    result.merge(acc)
    kwargs.setdefault("Propagators", [ prop ]) 
    kwargs.setdefault("ResolveMuonStation", False)
    acc = MuonExtrapolatorCfg(flags, name=name)
    result.setPrivateTools(acc.getPrimary())
    result.merge(acc)
    
    return result

def MCTBFitterCfg(flags, name='MCTBFitter', **kwargs):
    # didn't bother with MCTBSLFitter, since this seems redundant. Just set "StraightLine" = True since the kwargs are passed on to MuonChi2TrackFitterCfg  
    result = ComponentAccumulator()
    
    acc = MCTBExtrapolatorCfg(flags)
    mctbExtrapolator = acc.getPrimary()
    result.addPublicTool(mctbExtrapolator)
    result.merge(acc)
    
    kwargs.setdefault("ExtrapolationTool", mctbExtrapolator)
    kwargs.setdefault("GetMaterialFromTrack", True)
    kwargs.setdefault("Momentum", flags.Muon.straightLineFitMomentum)
    
    # extra_kwargs = {}
    # if 'StraightLine' in kwargs:
    #   # Pass this on! Can't safely just pass on kwargs, because MuonChi2TrackFitterCfg also has a property ExtrapolationTool
    #   extra_kwargs.setdefault('StraightLine', kwargs['StraightLine'])
    #   extra_kwargs.setdefault('GetMaterialFromTrack', kwargs['GetMaterialFromTrack'])
    acc = MuonChi2TrackFitterCfg(flags, name=name, **kwargs)
    mctbfitter = acc.getPrimary()
    result.merge(acc)
    # print mctbfitter
    result.setPrivateTools(mctbfitter)
    return result

def MuonPhiHitSelector(flags, name="MuonPhiHitSelector",**kwargs):
    MuonPhiHitSelector=CompFactory.MuonPhiHitSelector
    kwargs.setdefault("MakeClusters", True)
    kwargs.setdefault("CompetingRios", True)
    kwargs.setdefault("DoCosmics", flags.Beam.Type == 'cosmics')

    return MuonPhiHitSelector(name,**kwargs)


def MuPatHitToolCfg(flags, name="MuPatHitTool",**kwargs):
    Muon__MuPatHitTool=CompFactory.Muon.MuPatHitTool
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg,MdtDriftCircleOnTrackCreatorCfg
    
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_creator=result.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_creator)
    
    acc = CscClusterOnTrackCreatorCfg(flags)
    csc_cluster_creator = acc.popPrivateTools()
    result.merge(acc)
    kwargs.setdefault("CscRotCreator", csc_cluster_creator)
    
    result.setPrivateTools(Muon__MuPatHitTool(name,**kwargs))
    return result

def MuonRefitToolCfg(flags, name="MuonRefitTool", **kwargs):
    # FIXME - many tools are not yet explicitly configured here.
    result= ComponentAccumulator()
    result.setPrivateTools(CompFactory.Muon.MuonRefitTool(name, **kwargs))
    return result

