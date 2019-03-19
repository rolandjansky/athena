# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# This file configures the Muon segment finding. It is based on a few files in the old configuration system:
# Tools, which are configured here: 
# https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecTools.py
# https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py
# from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/CscTools.py
#
# and algorithms which are defined in several places:
# 
#
# When porting it I have tried to remove anything redundant, and I have tried to simplify the number of configuration techniques 
# used, so for example I'm no longer using CfgGetter, and am minimising the use of CfgMgr,

# There is now quite a lot in here not solely needed for SegmentFinding, so probably need to spit into another fragment.
# Core
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# Tracking
from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator, Trk__EnergyLossUpdator, Trk__MultipleScatteringUpdator, Trk__Navigator
from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__TrackSelectionProcessorTool
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool

# Muon
from CscSegmentMakers.CscSegmentMakersConf import CscSegmentUtilTool, Csc2dSegmentMaker, Csc4dSegmentMaker
from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__DCMathSegmentMaker, Muon__MdtMathSegmentFinder, Muon__MuonSegmentFittingTool, Muon__MuonClusterSegmentFinderTool
from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonTrackCleaner, Muon__MuonTrackScoringTool
from MuonCompetingClustersOnTrackCreator.MuonCompetingClustersOnTrackCreatorConf import Muon__TriggerChamberClusterOnTrackCreator
from MuonStationIntersectSvc.MuonStationIntersectSvcConf import MuonStationIntersectSvc
from MuonSegmentSelectionTools.MuonSegmentSelectionToolsConf import Muon__MuonSegmentSelectionTool, Muon__MuonSegmentHitSummaryTool
from MdtSegmentT0Fitter.MdtSegmentT0FitterConf import TrkDriftCircleMath__MdtSegmentT0Fitter
from MuonClusterSegmentMakerTools.MuonClusterSegmentMakerToolsConf import Muon__MuonClusterSegmentFinder
from MuonAmbiTrackSelectionTool.MuonAmbiTrackSelectionToolConf import Muon__MuonAmbiTrackSelectionTool

#Local
import MuonConfig.MuonRIO_OnTrackCreatorConfig # Trying to avoid circular dependencies here
from MuonConfig.MuonCondSvcConfig import MDTCondSummarySvcCfg
from MuonConfig.MuonCalibConfig import MdtCalibrationDbSvcCfg

def MuonTrackSummaryHelperToolCfg(flags, **kwargs):
    # m_muonTgTool("MuonHolesOnTrack"),
    #   m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    #   m_edmHelperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    #   m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    #   m_slExtrapolator("Trk::Extrapolator/MuonStraightLineExtrapolator"),
    
    result = ComponentAccumulator()
    acc, geom_svc = TrackingGeometrySvcCfg(flags)
    result.merge(acc)
    kwargs.setdefault("TrackingGeometryName", 'AtlasTrackingGeometry') # FIXME - get this from somewhere?
    
    acc, extrap = MuonExtrapolatorCfg(flags)
    acc.addPublicTool(extrap)
    result.merge(acc)
    kwargs.setdefault("Extrapolator", extrap)
    
    kwargs.setdefault("DoHolesOnTrack", False)
    kwargs.setdefault("CalculateCloseHits", True)

    from MuonTrackSummaryHelperTool.MuonTrackSummaryHelperToolConf import Muon__MuonTrackSummaryHelperTool
    
    return result, Muon__MuonTrackSummaryHelperTool(**kwargs)


def MuonTrackSummaryToolCfg(flags, **kwargs):
    result = ComponentAccumulator()
    acc, track_summary_helper = MuonTrackSummaryHelperToolCfg(flags)
    acc.addPublicTool(track_summary_helper)
    result.merge(acc)
    kwargs.setdefault("MuonSummaryHelperTool", track_summary_helper )
    kwargs.setdefault("doSharedHits", False )
    kwargs.setdefault("AddDetailedMuonSummary", True )
    return result, Trk__TrackSummaryTool(**kwargs)

def MuonTrackScoringToolCfg(flags, **kwargs):
    # m_trkSummaryTool("Trk::TrackSummaryTool"),    
    # m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    result = ComponentAccumulator()
    acc,muon_track_summary_tool = MuonTrackSummaryToolCfg(flags)
    acc.addPublicTool(muon_track_summary_tool)
    result.merge(acc)
    kwargs.setdefault('SumHelpTool', muon_track_summary_tool)
    scoring_tool = Muon__MuonTrackScoringTool(**kwargs)
    return result, scoring_tool

def MuonAmbiProcessorCfg(flags, **kwargs):
    #m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
    #m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool")
    result = ComponentAccumulator()
    kwargs.setdefault('DropDouble', False)
    acc, scoring_tool = MuonTrackScoringToolCfg( flags )
    acc.addPublicTool(scoring_tool)
    result.merge(acc)
    kwargs.setdefault('ScoringTool', scoring_tool )
    muon_ami_selection_tool = Muon__MuonAmbiTrackSelectionTool()
    result.addPublicTool(muon_ami_selection_tool)
    kwargs.setdefault('SelectionTool', muon_ami_selection_tool)

    return result, Trk__TrackSelectionProcessorTool(**kwargs)

# This probably doesn't belong here.
def MuonStationIntersectSvcCfg(flags, **kwargs):
    # Has dependency on MDTCondSummarySvc and IdHelperTool (which we ignore for now)
    result = ComponentAccumulator()
    acc, mdt_cond_summary_svc = MDTCondSummarySvcCfg(flags)
    result.merge(acc)
    kwargs.setdefault("MDTCondSummarySvc", mdt_cond_summary_svc)
    muon_station_intersect_svc = MuonStationIntersectSvc(**kwargs)
    result.addService(muon_station_intersect_svc)
    return result, muon_station_intersect_svc

# default muon navigator
def MuonNavigatorCfg(flags, **kwargs):
    result = ComponentAccumulator()

    acc, tracking_geom_service = TrackingGeometrySvcCfg(flags)
    result.merge(acc)
    kwargs.setdefault("TrackingGeometrySvc", tracking_geom_service)

    navigator = Trk__Navigator(**kwargs)
    result.addPublicTool(navigator)    
    return result,navigator

def MuonExtrapolatorCfg(flags,**kwargs):
    from TrkExTools.TrkExToolsConf import Trk__Extrapolator 
    result = ComponentAccumulator()
    
    energy_loss_updator = Trk__EnergyLossUpdator() # Not really sure these should be tools...
    result.addPublicTool(energy_loss_updator) # TODO remove 

    # This one has a dependency on RndNumberService
    mult_scat_updator = Trk__MultipleScatteringUpdator()
    result.addPublicTool(mult_scat_updator) # TODO remove 
    
    material_effects_updator = Trk__MaterialEffectsUpdator( EnergyLossUpdator=energy_loss_updator, MultipleScatteringUpdator=mult_scat_updator)
    result.addPublicTool(material_effects_updator)
    kwargs.setdefault("MaterialEffectsUpdators", [material_effects_updator])
    
    acc,navigator = MuonNavigatorCfg(flags)
    acc.addPublicTool(navigator)
    result.merge(acc)
    kwargs.setdefault("Navigator", navigator)
    
    muon_prop = Trk__STEP_Propagator(Tolerance = 0.00001, MaterialEffects=True, IncludeBgradients=True)
    acc.addPublicTool(muon_prop)
    
    kwargs.setdefault("Propagators", [muon_prop])
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("Tolerance", 0.0011)  # must be > 1um to avoid missing MTG intersections
    extrap = Trk__Extrapolator(**kwargs)
    return result, extrap

def MuonChi2TrackFitterCfg(flags,**kwargs):
    result = ComponentAccumulator()
    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter

    acc,extrapolator = MuonExtrapolatorCfg(flags)
    acc.addPublicTool(extrapolator) # TODO remove
    result.merge(acc)
    
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
    acc, extrapolator = MuonExtrapolatorCfg(flags)
    acc.addPublicTool(extrapolator) # TODO remove
    result.merge(acc)
    
    kwargs["ExtrapolationTool"] = extrapolator
    kwargs["PropagatorTool"]    = extrapolator.Propagators[0]
    kwargs["NavigatorTool"]     = extrapolator.Navigator

    fitter = Trk__GlobalChi2Fitter(**kwargs)
    result.addPublicTool(fitter)
    print fitter
    return result, fitter


def MCTBExtrapolatorCfg(name='MCTBExtrapolator',**kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("Propagators", ["MCTBPropagator"]) # FIXME!
    kwargs.setdefault("ResolveMuonStation", False)
    extrap = MuonExtrapolatorCfg(flags, **kwargs)
    return result, extrap

def MCTBFitterCfg(flags, **kwargs):
    # didn't bother with MCTBSLFitter, since this seems redundant. Just set "StraightLine" = True
    result = ComponentAccumulator()
    kwargs.setdefault("ExtrapolationTool", "MCTBExtrapolator")
    kwargs.setdefault("GetMaterialFromTrack", True)
    kwargs.setdefault("Momentum", flags.Muon.straightLineFitMomentum)
    acc, mctbfitter = MuonChi2TrackFitterCfg(flags, **kwargs)
    result.merge(acc)
    print mctbfitter
    return result,mctbfitter

def MuonHoughPatternFinderTool(flags, **kwargs):
    # Taken from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecTools.py#L173     from MuonHoughPatternTools/MuonHoughPatternToolsConf import Muon__MuonHoughPatternFinderTool

    from MuonHoughPatternTools.MuonHoughPatternToolsConf import Muon__MuonHoughPatternFinderTool
    
    # TODO
    # getPublicTool("MuonCombinePatternTool")
    # getPublicTool("MuonHoughPatternTool")
    if flags.Beam.Type == 'collisions':
        kwargs.setdefault("MDT_TDC_cut", False)
        kwargs.setdefault("RecordAll",False)
    return Muon__MuonHoughPatternFinderTool("MuonHoughPatternFinderTool",**kwargs)

def MuonCurvedSegmentCombiner(flags, **kwargs):  
    # Taken from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L74
    # The original code seems very odd. The default MissedHitsCut is 100 by default, and the rest of it is a bit tortuous.
    # I've tried to clean it up, but might have made mistakes. 
    from MuonCurvedSegmentCombiner.MuonCurvedSegmentCombinerConf import Muon__MuonCurvedSegmentCombiner 
    if flags.Beam.Type!= 'collisions' :
        kwargs.setdefault( "AddUnassociatedMiddleEndcapSegments", False )
    elif flags.Input.isMC:
        kwargs.setdefault( "MissedHitsCut", 4 )

    kwargs.setdefault("DoCosmics", flags.Beam.Type != 'collisions' )
    kwargs.setdefault( "AddAll2DCscs", False )
    kwargs.setdefault( "UseCscSegments", flags.Muon.doCSCs )
    kwargs.setdefault( "AddUnassociatedMiddleEndcapSegments", True )
    return Muon__MuonCurvedSegmentCombiner("MuonCurvedSegmentCombiner", **kwargs)
    
# def MuonSegmentCombinationCleanerTool(flags):

def MdtDriftCircleOnTrackCreatorAdjustableT0(flags,**kwargs):
    kwargs.setdefault("TimingMode", 3)
    kwargs.setdefault("DoTofCorrection", True)
    kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_data'))
    kwargs.setdefault("MuonTofTool", AdjustableT0Tool(flags))
    return MdtDriftCircleOnTrackCreator(name,**kwargs)

def AdjustableT0Tool(flags,**kwargs):
    # NB: the following 'ifs' are the same as in the MdtDriftCircleOnTrackCreator, so that the ToF setting is the same
    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("DoTof", 0)
    else: # collisions simulation final precise cuts
        kwargs.setdefault("DoTof", 1)
        
    from MdtDriftCircleOnTrackCreator.MdtDriftCircleOnTrackCreatorConf import AdjT0__AdjustableT0Tool
    return AdjT0__AdjustableT0Tool(**kwargs)

def MdtMathSegmentFinder(flags,**kwargs):
    # beamType       = getattr(extraFlags,"beamType", beamFlags.beamType())
    # doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit",muonRecFlags.doSegmentT0Fit())
    # enableCurvedSegmentFinding = getattr(extraFlags,"enableCurvedSegmentFinding", muonStandaloneFlags.enableCurvedSegmentFinding())

    if flags.Muon.doSegmentT0Fit:
        kwargs.setdefault("AssociationRoadWidth", 3.)
        kwargs.setdefault("MDTAssocationPullcut", 3.)
        kwargs.setdefault("RecoverMdtOutliers", False)
        kwargs.setdefault("DCFitProvider", "MdtSegmentT0Fitter" )

    if flags.Beam.Type == 'singlebeam' or flags.Beam.Type == 'cosmics' or flags.Input.isMC == False:
        kwargs.setdefault("AssociationRoadWidth", 2.)
        kwargs.setdefault("MDTAssocationPullcut", 4.)
        kwargs.setdefault("RecoverMdtOutliers", True )

    if flags.Muon.enableCurvedSegmentFinding:
        kwargs.setdefault("DoCurvedSegmentFinder",True)

    return Muon__MdtMathSegmentFinder(**kwargs)

def TriggerChamberClusterOnTrackCreatorCfg(flags, **kwargs):
    result=ComponentAccumulator()
    acc =  MuonConfig.MuonRIO_OnTrackCreatorConfig.MuonClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("ClusterCreator", muon_cluster_creator)
    result.addPublicTool(Muon__TriggerChamberClusterOnTrackCreator(**kwargs),primary=True)
    return result

def MuonTrackCleanerCfg(flags, **kwargs):
    # declareProperty("IdHelper",m_idHelper);
    # declareProperty("Helper",m_helper);
    # declareProperty("Printer",m_printer);
    # declareProperty("MdtRotCreator",  m_mdtRotCreator );
    # declareProperty("CompRotCreator", m_compRotCreator );
    # declareProperty("PullCalculator", m_pullCalculator );
    # declareProperty("Extrapolator",   m_extrapolator );
    # declareProperty("MagFieldSvc",    m_magFieldSvc );
    # declareProperty("MeasurementUpdator",   m_measurementUpdator );
    result=ComponentAccumulator()
    
    acc = MuonConfig.MuonRIO_OnTrackCreatorConfig.MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MdtRotCreator", mdt_creator)
    
    acc =  TriggerChamberClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CompRotCreator", muon_cluster_creator)
    
    # For PullCalculator, just let it get default for moment. FIXME
    
    acc,extrapolator = MuonExtrapolatorCfg(flags)
    acc.addPublicTool(extrapolator) # TODO remove
    result.merge(acc)
    kwargs.setdefault("Extrapolator", extrapolator)

    acc, slfitter = MCTBFitterCfg(flags, name = "SLFitter", StraightLine=True)
    acc.addPublicTool(slfitter)
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)

    acc, fitter = MCTBFitterCfg(flags, name = "CurvedFitter")
    acc.addPublicTool(fitter)
    result.merge(acc)
    kwargs.setdefault("Fitter", fitter)    

    # kwargs.setdefault("MagFieldSvc", mag_field_svc) Default for moment

    # FIXME - do remaining tools
    
    
    return result, Muon__MuonTrackCleaner(**kwargs)

def MuonSegmentFittingToolCfg(flags, **kwargs):
    # declareProperty("SLPropagator",   m_slPropagator);
    # declareProperty("SLFitter",       m_slTrackFitter);
    # declareProperty("CurvedFitter",   m_curvedTrackFitter);
    # declareProperty("TrackCleaner",   m_trackCleaner);
    # declareProperty("IdHelper",       m_idHelperTool);
    # declareProperty("UpdatePrecisionCoordinate", m_updatePrecisionCoordinate = false );
    result=ComponentAccumulator()
    # FIXME! Add this.
    # acc, propagator = AtlasRungeKuttaPropagatorCfg(flags)
    # result.merge(acc)
    # kwargs.setdefault("SLPropagator", propagator)
    
    acc, slfitter = MCTBFitterCfg(flags, name = "SLFitter", StraightLine=True)
    acc.addPublicTool(slfitter)
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)
    
    acc, fitter = MCTBFitterCfg(flags, name = "CurvedFitter")
    acc.addPublicTool(fitter)
    result.merge(acc)
    kwargs.setdefault("CurvedFitter", fitter)
    
    acc, cleaner = MuonTrackCleanerCfg(flags)
    acc.addPublicTool(cleaner)
    result.merge(acc)
    kwargs.setdefault("TrackCleaner", cleaner)
    result.addPublicTool(Muon__MuonSegmentFittingTool(**kwargs))
    return result

def DCMathSegmentMakerCfg(flags, **kwargs):    
    # This in general is a pretty problematic piece of code. It seems to have a lot of potential issues, because it has loads of mutables / subtools etc
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/DCMathSegmentMaker/src/DCMathSegmentMaker.h
    # ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreator;         //<! mdt rio ontrack creator
    # ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreatorT0;       //<! mdt rio ontrack creator
    # ToolHandle<IMuonClusterOnTrackCreator>    m_clusterCreator;     //<! cluster rio ontrack creator
    # ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compClusterCreator;   //<! competing clusters rio ontrack creator
    # ToolHandle<MuonIdHelperTool>              m_idHelperTool;    //<! Id helper tool
    # ToolHandle<MuonEDMPrinterTool>            m_printer;         //<! printer helper tool
    # ToolHandle<MuonEDMHelperTool>             m_helper;          //<! printer helper tool
    # ToolHandle<IMdtSegmentFinder>             m_segmentFinder;   //<! segment finder tool MdtSegmentFinder
    # mutable ToolHandle<AdjT0::IAdjustableT0Tool>      m_tofTool;         //<! tof tool
    # ToolHandle<IMuonSegmentFittingTool>       m_segmentFitter;   //<! segment fitting tool
    # ToolHandle<IMuonSegmentSelectionTool>     m_segmentSelectionTool; //<! segment selection tool
    # ToolHandle<IDCSLFitProvider>              m_dcslFitProvider;

    result=ComponentAccumulator()
    
    beamType       = flags.Beam.Type
    doSegmentT0Fit = flags.Muon.doSegmentT0Fit
    updateSegmentSecondCoordinate = flags.Muon.updateSegmentSecondCoordinate
    enableCurvedSegmentFinding = flags.Muon.enableCurvedSegmentFinding
        
    kwargs.setdefault("RefitSegment", True)
    kwargs.setdefault("AssumePointingPhi", beamType != 'cosmics')
    kwargs.setdefault("OutputFittedT0", True)

    acc = MdtCalibrationDbSvcCfg(flags) # Needed by MdtSegmentT0Fitter
    result.merge(acc)
    mdt_segment_t0_fitter = TrkDriftCircleMath__MdtSegmentT0Fitter()
    result.addPublicTool(mdt_segment_t0_fitter)
    kwargs.setdefault("DCFitProvider", mdt_segment_t0_fitter)
    #kwargs.setdefault("CurvedErrorScaling", False)
    kwargs.setdefault("UsePreciseError", True)
    kwargs.setdefault("SinAngleCut", 0.4)

    if (beamType == 'singlebeam' or beamType == 'cosmics'): 
        kwargs.setdefault("SinAngleCut", 0.9)
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)
        kwargs.setdefault("CurvedErrorScaling", False)
    elif not flags.Input.isMC: # collisions real data 
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)

    if doSegmentT0Fit:
        tof_tool = AdjustableT0Tool(flags)
        result.merge(MuonConfig.MuonRIO_OnTrackCreatorConfig.MdtDriftCircleOnTrackCreatorCfg(flags, name="MdtDriftCircleOnTrackCreatorAdjustableT0", TimingMode=3, \
                   DoTofCorrection=True, TimeWindowSetting=mdtCalibWindowNumber('Collision_data'), MuonTofTool=tof_tool))

        kwargs.setdefault("MdtCreatorT0", mdt_creator) # TODO - is this correct? 
        kwargs.setdefault("TofTool", tof_tool(flags))
        mdt_math_segment_finder = MdtMathSegmentFinder(flags, doSegmentT0Fit=True)
    else:
        mdt_math_segment_finder = MdtMathSegmentFinder(flags)
        
    result.addPublicTool(mdt_math_segment_finder)
    kwargs.setdefault("MdtSegmentFinder", mdt_math_segment_finder )

    if updateSegmentSecondCoordinate:
        kwargs.setdefault("UpdatePhiUsingPhiHits",True)

    if enableCurvedSegmentFinding:
        kwargs.setdefault("CurvedErrorScaling", False)
        kwargs.setdefault("PreciseErrorScale", 1)
        kwargs.setdefault("UsePreciseError", True)
    
    # Now stuff that wasn't explicitly configured before.
    
    acc, muon_station_intersect_svc = MuonStationIntersectSvcCfg(flags)
    result.merge(acc)
    kwargs.setdefault("MuonStationIntersectSvc", muon_station_intersect_svc)
    
    acc=MuonConfig.MuonRIO_OnTrackCreatorConfig.MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtCreator", acc.getPrimary())
    result.merge(acc)

    
    acc=MuonConfig.MuonRIO_OnTrackCreatorConfig.MuonClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    # FIXME - declare property  for muon_cluster_creator is missing from the tool.
    
    muon_comp_cluster_creator =  Muon__TriggerChamberClusterOnTrackCreator(ClusterCreator=muon_cluster_creator)
    acc.addPublicTool(muon_comp_cluster_creator)
    kwargs.setdefault("MuonCompetingClustersCreator", muon_comp_cluster_creator)

    acc =  MuonSegmentFittingToolCfg(flags, name="NewMuonSegmentFittingTool")
    segment_fitter=acc.getPrimary()
    print segment_fitter
    
    result.merge(acc)    
    kwargs.setdefault("SegmentFitter", segment_fitter)
    
    segment_selector =  Muon__MuonSegmentSelectionTool()
    result.addPublicTool(segment_selector)
    kwargs.setdefault("SegmentSelector", segment_selector)
    
    dc_segment_maker = Muon__DCMathSegmentMaker(**kwargs)
    
    return result, dc_segment_maker


def MuonPatternSegmentMakerCfg(flags, **kwargs):
    # Taken from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L49
    
    # Tool has the following subtools:
    # DCMathSegmentMaker, MdtDriftCircleOnTrackCreator, MuonClusterOnTrackCreator, MuonEDMPrinterTool, MuonIdHelperTool
    result=ComponentAccumulator()
    from MuonPatternSegmentMaker.MuonPatternSegmentMakerConf import Muon__MuonPatternSegmentMaker
    from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber # TODO - should maybe move this somewhere else?
    
    if "MdtCreator" not in kwargs: 
        # on data configure a MdtDriftCircleOnTrackCreator for the segment finding with reduced errors
        # when using the t0 refit enlarge the time window
        if not flags.Input.isMC and flags.Beam.Type == 'collisions':
            if flags.Muon.doSegmentT0Fit:
                timeWindowSetting = mdtCalibWindowNumber('Collision_t0fit')
            else:
                timeWindowSetting = mdtCalibWindowNumber('Collision_data')
            acc = MuonConfig.MuonRIO_OnTrackCreatorConfig.MdtDriftCircleOnTrackCreatorCfg(flags, name="MdtDriftCircleOnTrackCreator_NoTubeHits", CreateTubeHit = False, TimeWindowSetting = timeWindowSetting)   
        else:
            # I think we need to configure a 'default' version of the MdtDriftCircleOnTrackCreator here
            acc=MuonConfig.MuonRIO_OnTrackCreatorConfig.MdtDriftCircleOnTrackCreatorCfg(flags)

        result.merge(acc)
        kwargs.setdefault('MdtCreator', acc.getPrimary())
    #else:
    # TODO work out what to do here
    
    acc = MuonConfig.MuonRIO_OnTrackCreatorConfig.MuonClusterOnTrackCreatorCfg(flags)  
    kwargs.setdefault('ClusterCreator', acc.getPrimary())
    result.merge(acc)
    
    # Other dependencies:
    # EDM printer tool, MuonIdHelperTool

    acc, segment_maker = DCMathSegmentMakerCfg(flags,name="NewDCMathSegmentMaker")
    acc.addPublicTool(segment_maker)
    kwargs.setdefault('SegmentMaker', segment_maker)
    result.merge(acc)

    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("AngleCutPhi", 1e9)
        kwargs.setdefault("DropDistance", 100000000.)
    
    return result, Muon__MuonPatternSegmentMaker("MuonPatternSegmentMaker", **kwargs)
    
def CscAlignmentTool(flags, **kwargs):
    from CscClusterization.CscClusterizationConf import CscAlignmentTool
    etaposAlignConsts = [ # 1st, 2nd, 3rd, 4th layer 
        0.0,   -0.0902347,   -0.0984321,    -0.141175, #sector -16
        0.0,    -0.166412,    -0.150399,     -0.18592, #sector -15
        0.0,   -0.0544449,     0.101448,   -0.0433321, #sector -14
        0.0,    -0.047769,    -0.156247,   -0.0854826, #sector -13
        0.0,   -0.0970101,    -0.227578,    -0.301079, #sector -12
        0.0,    -0.152912,    -0.365218,    -0.314042, #sector -11
        0.0,    0.0338696,    -0.264766,    -0.224567, #sector -10
        0.0,    -0.107372,     -0.16099,     -0.13508, #sector -9
        0.0,   -0.0663979,   -0.0312645,    -0.130711, #sector -8
        0.0,    -0.249974,    -0.269372,    -0.353648, #sector -7
        0.0,    -0.194175,    0.0238348,    -0.188433, #sector -6
        0.0,    0.0977346,    0.0492461,     0.150434, #sector -5
        0.0,    0.0444637,   -0.0871214,   -0.0627541, #sector -4
        0.0,    0.0347245,   -0.0226186,   -0.0826478, #sector -3
        0.0,  -0.00408879,   -0.0638005,    -0.206868, #sector -2
        0.0,    0.0331328,    0.0402103,     0.118611, #sector -1
        0.0,   -0.0634537,   -0.0516743,    0.0045364, #sector 1
        0.0,   -0.0957718,    -0.235246,    -0.439811, #sector 2
        0.0,    -0.163504,   -0.0129115,   0.00140143, #sector 3
        0.0,    -0.182903,   -0.0503858,    -0.207799, #sector 4
        0.0,   -0.0970927,            0,     0.124744, #sector 5
        0.0,   -0.0169602,   -0.0527447,    0.0319154, #sector 6
        0.0,    -0.176499,    -0.196542,   -0.0846979, #sector 7
        0.0,    0.0589994,    0.0131431,    0.0461769, #sector 8
        0.0,   -0.0237507,   -0.0307316,     0.144429, #sector 9
        0.0,   -0.0144483,   -0.0328234,  -0.00553684, #sector 10
        0.0,     0.113188,    -0.176182,    0.0635706, #sector 11
        0.0,    -0.109776,    -0.727705,    -0.756824, #sector 12
        0.0,   -0.0553061,    -0.056162,   -0.0336955, #sector 13
        0.0,    -0.070077,    -0.127203,    -0.140082, #sector 14
        0.0,   -0.0844368,   -0.0735331,    -0.016667, #sector 15
        0.0,    0.0426443,    -0.153659,    -0.129711, #sector 16
    ]

    phiposAlignConsts = [ # 1st, 2nd, 3rd, 4th layer 
        0.0, 0.0, 0.0, 0.0, #-16
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-14
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-12
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-10
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-8
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-6
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, #-4
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-2
        0.0, 0.0, 0.0, 0.0, #-1  
        0.0, 0.0, 0.0, 0.0, # sector =1 and above...
        0.0, 0.0, 0.0, 0.0, #2
        0.0, 0.0, 0.0, 0.0, #3
        0.0, 0.0, 0.0, 0.0, #4
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, #6
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #8
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #10
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #12
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #14
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0  #16
    ]
    kwargs.setdefault("phiposAlignConsts", phiposAlignConsts)
    kwargs.setdefault("etaposAlignConsts", etaposAlignConsts)
    # kwargs.setdefault("useAlignment", flags.global.isMC is False and flags.muonAlign.UseIlines is False  )

    return CscAlignmentTool(**kwargs)
    

def QratCscClusterFitterCfg(flags, **kwargs):
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/CscTools.py#L151
    from CscClusterization.CscClusterizationConf import QratCscClusterFitter
    result=ComponentAccumulator()
    qratcor_css_eta = [
        0.000000, 0.000000, 0.000000, 0.000000, 0.0699381, 0.178291, 0.271303, 0.345611, 
        0.400738, 0.452451, 0.493772, 0.526792, 0.553845, 0.580111, 0.603337, 0.624749, 
        0.646065, 0.663924, 0.681897, 0.701345, 0.716067, 0.730324, 0.743168, 0.760598, 
        0.773341, 0.786406, 0.798358, 0.811339, 0.820949, 0.832676, 0.843917, 0.85412, 
        0.863914, 0.873229, 0.88251, 0.892856, 0.903296, 0.910067, 0.917892, 0.928028, 
        0.93461, 0.943679, 0.951617, 0.958326, 0.964428, 0.97341, 0.978204, 0.987075, 
        0.994789, 1.000000
        ]
    qratcor_csl_eta = [
        0.000000, 0.000000, 0.000000, 0.000000, 0.0290177, 0.0834867, 0.188683, 0.269967, 
        0.334887, 0.393036, 0.440317, 0.481884, 0.51725, 0.545107, 0.56959, 0.592327, 
        0.614087, 0.635344, 0.655252, 0.672766, 0.690929, 0.707293, 0.722742, 0.738323, 
        0.753562, 0.76749, 0.780983, 0.79354, 0.806521, 0.818085, 0.829987, 0.841443, 
        0.853723, 0.863393, 0.87431, 0.883139, 0.892962, 0.902803, 0.911276, 0.921417, 
        0.929387, 0.938106, 0.947072, 0.954406, 0.961692, 0.97055, 0.978517, 0.985133, 
        0.992257, 1.000000
        ]
    kwargs.setdefault("qratcor_csl_eta",qratcor_csl_eta)
    kwargs.setdefault("qratcor_css_eta",qratcor_css_eta)
    csc_align_tool = CscAlignmentTool(flags)
    result.addPublicTool(csc_align_tool) # TODO remove once private
    kwargs.setdefault("CscAlignmentTool", csc_align_tool )
    return result,QratCscClusterFitter(**kwargs)

def CscSegmentUtilToolCfg(flags, **kwargs):
    result=ComponentAccumulator()
    acc, qrat = QratCscClusterFitterCfg(flags)
    result.merge(acc)
    kwargs.setdefault("precision_fitter",   )
    acc, csc_cluster_creator = MuonConfig.MuonRIO_OnTrackCreatorConfig.CscClusterOnTrackCreatorCfg(flags)
    result.merge(acc)
    kwargs.setdefault("rot_creator", csc_cluster_creator )
    return result,CscSegmentUtilToolCfg(flags)

def Csc2dSegmentMakerCfg(flags, **kwargs):
    result=ComponentAccumulator()
    acc, csc_segment_util_tool = CscSegmentUtilToolCfg(flags)
    result.merge(acc)
    acc, csc_segment_maker = Csc2dSegmentMaker("Csc2dSegmentMaker", 
           segmentTool=csc_segment_util_tool)
    result.merge(acc)
    return result, csc_segment_maker 

def Csc4dSegmentMakerCfg(flags, **kwargs):
    result=ComponentAccumulator()
    acc, csc_segment_util_tool = CscSegmentUtilToolCfg(flags)
    result.merge(acc)
    acc, csc_segment_maker = Csc4dSegmentMaker("Csc4dSegmentMaker", 
           segmentTool=csc_segment_util_tool) 
    result.merge(acc)
    return result, csc_segment_maker

def MooSegmentFinderCfg(flags, **kwargs):
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L99 
    
    from MuonHoughPatternTools.MuonHoughPatternToolsConf import Muon__MuonLayerHoughTool
    from MuonSegmentOverlapRemovalTools.MuonSegmentOverlapRemovalToolsConf import Muon__MuonSegmentCombinationCleanerTool
    from MooSegmentCombinationFinder.MooSegmentCombinationFinderConf import Muon__MooSegmentCombinationFinder

    result=ComponentAccumulator()

    # Need muon geometry to work!
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg 
    result.merge( MuonGeoModelCfg(flags) )    

    muon_curved_segment_combiner_tool = MuonCurvedSegmentCombiner(flags)
    result.addPublicTool(muon_curved_segment_combiner_tool)
    
    muon_segment_combination_cleaner_tool = Muon__MuonSegmentCombinationCleanerTool()
    result.addPublicTool(muon_segment_combination_cleaner_tool)
    
    # Use the MuonLayerHoughTool for collisions, MuonHoughPatternFinderTool for everything else.
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecTools.py#L428
    muon_pattern_finder_tool = Muon__MuonLayerHoughTool("MuonLayerHoughTool", DoTruth= flags.Input.isMC) \
                              if flags.Beam.Type=="collisions" else MuonHoughPatternFinderTool(flags)
    result.addPublicTool(muon_pattern_finder_tool)
    
    acc, muon_pattern_segment_maker = MuonPatternSegmentMakerCfg(flags)
    result.merge(acc)
    result.addPublicTool(muon_pattern_segment_maker)
    
    if flags.Muon.doCSCs:
        acc,csc_2d_segment_maker = Csc2dSegmentMakerCfg(flags)
        result.merge(acc)
    
        acc,csc_4d_segment_maker = Csc4dSegmentMakerCfg(flags)
        result.merge(acc)
    else:
        csc_2d_segment_maker=None
        csc_4d_segment_maker=None
    
    segment_finder_tool = Muon__MooSegmentCombinationFinder("NewMooSegmentCombinationFinder", 
        SegmentCombiner = muon_curved_segment_combiner_tool,
        SegmentCombinationCleaner = muon_segment_combination_cleaner_tool,
        HoughPatternFinder =muon_pattern_finder_tool,
        MdtSegmentMaker=muon_pattern_segment_maker,
        DoSegmentCombinations=False,
        DoSegmentCombinationCleaning=False,
        DoCscSegments = flags.Muon.doCSCs,
        DoMdtSegments = flags.Muon.doMDTs,   
        Csc2dSegmentMaker = csc_2d_segment_maker,
        Csc4dSegmentMaker = csc_4d_segment_maker,
        DoSummary = flags.Muon.printSummary
        )
    
    result.setPrivateTools(segment_finder_tool)
    return result

def MuonClusterSegmentFinderToolCfg(flags, **kwargs):
    #m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitter"),
    #m_ambiTool("Trk::SimpleAmbiguityProcessorTool/MuonAmbiProcessor"),
    #m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    #m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    #m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    #m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    #m_trackCleaner("Muon::MuonTrackCleaner/MuonTrackCleaner") {
    #declareProperty("SLFitter",            m_slTrackFitter);
    #declareProperty("SegmentAmbiguityTool",m_ambiTool);
    #declareProperty("TrackToSegmentTool",  m_trackToSegmentTool);
    #declareProperty("IdHelper",            m_idHelperTool);
    #declareProperty("TrackCleaner",        m_trackCleaner);
    result=ComponentAccumulator()

    acc, slfitter = MCTBFitterCfg(flags, name = "SLFitter", StraightLine=True)
    acc.addPublicTool(slfitter)
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)

    acc, cleaner = MuonTrackCleanerCfg(flags)
    acc.addPublicTool(cleaner)
    result.merge(acc)
    kwargs.setdefault("TrackCleaner", cleaner)
    
    # FIXME - remaining tools
    acc.setPrivateTools(Muon__MuonClusterSegmentFinderTool(**kwargs))
    return acc
    
def MuonClusterSegmentFinderCfg(flags, **kwargs):
    #declareProperty("MuonClusterizationTool", m_clusterTool);
    #declareProperty("MuonIdHelperTool",m_idHelper );    
    #declareProperty("MuonEDMPrinterTool",m_printer );    
    #declareProperty("MuonPRDSelectionTool", m_muonPRDSelectionTool );
    #declareProperty("MdtSegmentMaker",m_segmentMaker);
    #declareProperty("SLFitter",            m_slTrackFitter);
    #declareProperty("TrackToSegmentTool",  m_trackToSegmentTool);
    #declareProperty("AmbiguityProcessor",m_ambiguityProcessor);
    #declareProperty("TrackCleaner",        m_trackCleaner);
    result=ComponentAccumulator()
    # FIXME - rest of the tools.

    acc,segment_maker = DCMathSegmentMakerCfg(flags,name="NewDCMathSegmentMaker")
    acc.addPublicTool(segment_maker)
    kwargs.setdefault('MdtSegmentMaker', segment_maker)
    result.merge(acc)

    acc, slfitter = MCTBFitterCfg(flags, name = "SLFitter", StraightLine=True)
    acc.addPublicTool(slfitter)
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)
    
    acc, ambi  = MuonAmbiProcessorCfg(flags, name='NewMuonAmbiProcessor')
    acc.addPublicTool(ambi)
    result.merge(acc)
    kwargs.setdefault("AmbiguityProcessor", ambi)

    acc,cleaner = MuonTrackCleanerCfg(flags)
    acc.addPublicTool(cleaner)
    result.merge(acc)
    kwargs.setdefault("TrackCleaner", cleaner)

    result.addPublicTool(Muon__MuonClusterSegmentFinder(**kwargs),primary=True)
    return result

def MuonSegmentFindingCfg(flags, **kwargs):
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py#L113
    from MooSegmentCombinationFinder.MooSegmentCombinationFinderConf import MooSegmentFinderAlg
    result=ComponentAccumulator()
    
    segment_location = "ThirdChainSegments" if flags.Muon.segmentOrigin=="TruthTracking" else "MuonSegments"
    
    acc  = MooSegmentFinderCfg(flags)
    segment_finder_tool=(acc.popPrivateTools())
    result.addPublicTool(segment_finder_tool)
    result.merge(acc)


    acc = MuonClusterSegmentFinderCfg(flags)
    muon_cluster_segment_finder=acc.getPrimary()
    result.merge(acc)

    moo_segment_finder_alg = MooSegmentFinderAlg( "MuonSegmentMaker",
                 SegmentFinder = segment_finder_tool,
                 MuonClusterSegmentFinderTool=muon_cluster_segment_finder,
                 MuonSegmentOutputLocation = segment_location,
                 UseCSC = flags.Muon.doCSCs,
                 UseMDT = flags.Muon.doMDTs,
                 UseRPC = flags.Muon.doRPCs,
                 UseTGC = flags.Muon.doTGCs,
                 UseTGCPriorBC = flags.Muon.doTGCs and flags.Muon.useTGCPriorNextBC,
                 UseTGCNextBC  = flags.Muon.doTGCs and flags.Muon.useTGCPriorNextBC,
                 doTGCClust = flags.Muon.doTGCClusterSegmentFinding,
                 doRPCClust = flags.Muon.doRPCClusterSegmentFinding )
    result.addEventAlgo( moo_segment_finder_alg )
        
    # TODO - add NCB stuff
    return result
    

if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonSegmentFindingConfig.py
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    log.setLevel(DEBUG)
    from AthenaCommon.Logging import log
    log.debug('About to set up Segment Finding.')
    
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.3/v1/myESD.pool.root"]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Muon.doCSCs = False 
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    # This is a temporary fix! Should be private!
    from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMHelperTool
    muon_edm_helper_tool = Muon__MuonEDMHelperTool("MuonEDMHelperTool", MuonExtrapolator=None) # Don't bring in this dependency yet!
    cfg.addPublicTool( muon_edm_helper_tool )

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge(MuonSegmentFindingCfg(ConfigFlags))
              
    f=open("MuonSegmentFinding.pkl","w")
    cfg.store(f)
    f.close()
